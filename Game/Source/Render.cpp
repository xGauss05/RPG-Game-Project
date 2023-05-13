#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Input.h"
#include "Map.h"

#include "Log.h"

#include <string>
#include <array>
#include <algorithm>
#include <numbers>		// std::numbers::pi

constexpr auto ticks_for_next_frame = (1000 / 60);
constexpr auto fps_UI_seconds_interval = 1.0f;

Render::Render() : Module()
{
	name = "render";
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render() = default;

// Called before render is available
bool Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");

	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if (vSyncActive = config.child("vsync").attribute("value").as_bool(); 
		vSyncActive)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	vSyncOnRestart = vSyncActive;

	std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>>renderPtr(
		SDL_CreateRenderer(app->win->GetWindow(), -1, flags),
		[](SDL_Renderer *r) { if(r) SDL_DestroyRenderer(r); }
	);
	
	renderer = std::move(renderPtr);
		
	if(!renderer)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	camera = {
			.x = 0,
			.y = 0,
			.w = app->win->GetSurface()->w,
			.h = app->win->GetSurface()->h
		};
	
	ticksForNextFrame = 1000/fpsTarget;

	return true;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// Background
	SDL_RenderGetViewport(renderer.get(), &viewport);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	if(!vSyncActive)
	{
		while(SDL_GetTicks() - renderLastTime < ticksForNextFrame)
		{
			SDL_Delay(1);
		}
	}
	
	SDL_RenderClear(renderer.get());
	return true;
}

bool Render::Update(float dt)
{
	using enum KeyState;

	int cameraSpeed = 1;
	if(app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
		cameraSpeed *= 10;
	if(app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		cameraSpeed *= 100;
	if(app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		cameraSpeed *= 250;

	if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		camera.y += cameraSpeed;

	if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		camera.y -= cameraSpeed;

	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		camera.x += cameraSpeed;

	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		camera.x -= cameraSpeed;

	return true;
}

bool Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer.get(),
						   background.r,
						   background.g,
						   background.g,
						   background.a
	);
	
	SDL_RenderPresent(renderer.get());
	
	// I -> increases fps target || O ->decreases fps target
	/*if(app->input->GetKey(SDL_SCANCODE_I) == KeyState::KEY_DOWN && fpsTarget < 1000)
	{
		fpsTarget += 10;
		ticksForNextFrame = 1000/fpsTarget;
	}
	if(app->input->GetKey(SDL_SCANCODE_K) == KeyState::KEY_DOWN && fpsTarget > 10)
	{
		fpsTarget -= 10;
		ticksForNextFrame = 1000/fpsTarget;
	}*/
	if(app->input->GetKey(SDL_SCANCODE_F11) == KeyState::KEY_DOWN)
	{
		if(fpsTarget != 30)
		{
			prevFPSTarget = fpsTarget;
			fpsTarget = 30;
		}
		else
		{
			fpsTarget = prevFPSTarget;
			prevFPSTarget = 30;
		}
		ticksForNextFrame = 1000/fpsTarget;
	}
	
	if(!vSyncActive) renderLastTime = SDL_GetTicks();
	
	fpsCounter++;
	if(fpsLast < SDL_GetTicks() - static_cast<uint32>(fps_UI_seconds_interval * 1000))
	{
		fpsLast = SDL_GetTicks();
		fps = fpsCounter;
		fpsCounter = 0;
	}
	return true;
}

bool Render::Pause(int phase)
{
	switch(phase)
	{
		case 1:
			return PreUpdate();
		case 3:
			return PostUpdate();
		default:
			return true;
	}
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect) const
{
	SDL_RenderSetViewport(renderer.get(), &rect);
}

void Render::ResetViewPort() const
{
	SDL_RenderSetViewport(renderer.get(), &viewport);
}

bool Render::DrawTexture(DrawParameters const &params) const
{
	if (params.textureID < 0 || params.textureID > app->tex->textures.size())
	{
		LOG("Texture ID is invalid. Cannot blit to screen.");
		return false;
	}

	auto texture = app->GetTexture(params.textureID);

	fPoint scale = (params.scale.IsZero())
		? fPoint(app->win->GetScale(), app->win->GetScale())
		: params.scale;

	SDL_Rect rect{ 0 };
	// (camera * parallaxSpeed) + position
	rect.x =
		static_cast<int>(std::floor(static_cast<float>(camera.x) * params.parallaxSpeed.x))
		+ params.position.x;
	rect.y =
		static_cast<int>(std::floor(static_cast<float>(camera.y) * params.parallaxSpeed.y))
		+ params.position.y;

	if (params.rectOffset != iPoint(INT_MAX, INT_MAX))
	{
		rect.x -= params.rectOffset.x;
		rect.y -= params.rectOffset.y;
	}

	if (params.section)
	{
		rect.w = params.section->w;
		rect.h = params.section->h;
	}
	else
	{
		SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
	}

	if (scale.x > 1.0f)
	{
		rect.w = static_cast<int>(std::floor(static_cast<float>(rect.w) * scale.x));
		//rect.x -= (rect.w / 2);
	}
	if (scale.y > 1.0f)
	{
		rect.h = static_cast<int>(std::floor(static_cast<float>(rect.h) * scale.y));
		//rect.y -= (rect.h / 2);
	}

	SDL_Point const* center = nullptr;

	if (params.center.x != INT_MAX && params.center.y != INT_MAX)
	{
		SDL_Point pivot{ params.center.x, params.center.y };
		center = &pivot;
	}

	if (SDL_RenderCopyEx(renderer.get(), texture, params.section, &rect, params.rotationAngle, center, params.flip))
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool Render::DrawShape(const SDL_Rect& rect, bool filled, SDL_Color color, bool use_camera, SDL_BlendMode blendMode) const
{
	SDL_SetRenderDrawBlendMode(renderer.get(), blendMode);
	SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);

	SDL_Rect rec(rect);
	
	if(use_camera)
	{	
		float scale = app->win->GetScale();
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}
	
	auto result = filled ? SDL_RenderFillRect(renderer.get(), &rec) : SDL_RenderDrawRect(renderer.get(), &rec);

	if(result)  
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool Render::DrawShape(iPoint v1, iPoint v2, SDL_Color color, bool use_camera, SDL_BlendMode blendMode) const
{

	SDL_SetRenderDrawBlendMode(renderer.get(), blendMode);
	SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);
	
	float scale = app->win->GetScale();
	
	iPoint cameraPos = use_camera ? iPoint{camera.x, camera.y} : iPoint{0, 0};
	
	iPoint v1Final = v1 * scale + cameraPos;
	iPoint v2Final = v2 * scale + cameraPos;

	if(SDL_RenderDrawLine(renderer.get(), v1Final.x, v1Final.y, v2Final.x, v2Final.y))
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool Render::DrawShape(iPoint center, int radius, SDL_Color color, bool use_camera, SDL_BlendMode blendMode) const
{
	[[maybe_unused]] float scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer.get(), blendMode);
	SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);

	std::array<SDL_Point, 360> points{};
	
	auto factor = std::numbers::pi_v<float> / 180.0f;
	auto r = static_cast<float>(radius);
	SDL_Point cam = use_camera ? SDL_Point(camera.x, camera.y) : SDL_Point(0,0);

	for(int i = 0; auto &elem : points)
	{
		auto offset = static_cast<float>(i) * factor;
		elem = {
			.x = center.x + cam.x + static_cast<int>(r * cos(offset)),
			.y = center.y + cam.y + static_cast<int>(r * sin(offset))
		};
		i++;
	}
	
	if(SDL_RenderDrawPoints(renderer.get(), points.data(), 360))
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool Render::LoadState(pugi::xml_node const &data)
{
	
	vSyncOnRestart = data.child("graphics").attribute("vsync").as_bool();
	fpsTarget = data.child("graphics").attribute("targetfps").as_uint();

	return true;
}

pugi::xml_node Render::SaveState(pugi::xml_node const &data) const
{
	pugi::xml_node cam = data;
	cam = cam.append_child("render");
	
	cam.append_child("graphics").append_attribute("vsync").set_value(vSyncOnRestart ? "true" : "false");
	cam.child("graphics").append_attribute("targetfps").set_value(std::to_string(fpsTarget).c_str());

	return cam;
}

bool Render::HasSaveData() const
{
	return true;
}

void Render::SetMapAndTileSize(iPoint mSize, iPoint tSize)
{
	mapSize = mSize;
	tileSize = tSize;
}

void Render::ToggleVSync()
{
	vSyncOnRestart = !vSyncOnRestart;
	app->SaveAttributeToConfig(name, "vsync", "value", vSyncOnRestart ? "true" : "false");
}

SDL_Rect Render::GetCamera() const
{
	return camera;
}

void Render::AdjustCamera(iPoint position)
{
	camera.x = (-1 * position.x) + camera.w/2;
	camera.y = (-1 * position.y) + camera.h/2;
}

void Render::ResetCamera()
{
	camera.x = 0;
	camera.y = 0;
}

SDL_Renderer* Render::GetRender() const
{
	return renderer.get();
}

void Render::StartEasing(std::string_view name)
{
	for (auto& elem : easings)
	{
		if (StrEquals(elem.name, name))
		{
			elem.Start();
			return;
		}
	}
}

void Render::InitEasings(pugi::xml_node const& node)
{
	easings.clear();

	for (auto const& easing : node.children("image"))
	{
		easings.emplace_back(easing);
	}
}

int Render::AddEasing(float totalTime)
{
	Easing easing;
	easing.SetTotalTime(totalTime);

	easings.push_back(easing);

	return 0;
}

bool Render::DrawEasing(int textureID, std::string_view name)
{
	for (auto& elem : easings)
	{
		// If it's not the easing we are looking for we go to next element
		if (!StrEquals(elem.name, name))
			continue;

		// If it hasn't started we don't draw anything and can exit function
		if (!elem.GetStarted())
			break;

		// If the element has finished the easing, we draw it in its final place
		if (elem.GetFinished()) [[likely]]
		{
			DrawTexture(DrawParameters(textureID, elem.targetPos));
		}
		// If it hasn't, we calculate the position and draw it
		else [[unlikely]]
		{
			double t = elem.TrackTime(app->dt);
			double easedX = elem.EasingAnimation(elem.startingPos.x, elem.targetPos.x, t, elem.type);
			double easedY = elem.EasingAnimation(elem.startingPos.y, elem.targetPos.y, t, elem.type);
			iPoint newPosition(easedX, easedY);
			DrawTexture(DrawParameters(textureID, newPosition));
		}
	}

	return true;
}

bool Render::EasingHasFinished(std::string_view name)
{
	for (auto const& elem : easings)
	{
		if (StrEquals(elem.name, name))
		{
			return elem.GetStarted() && elem.GetFinished();
		}
	}
	return false;
}

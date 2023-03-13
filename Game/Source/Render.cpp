#include "App.h"
#include "Window.h"
#include "Render.h"
#include "EntityManager.h"
#include "Input.h"
#include "Map.h"

#include "Defs.h"
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
	if(app->input->GetKey(SDL_SCANCODE_V) == KeyState::KEY_DOWN)
	{
		vSyncOnRestart = !vSyncOnRestart;
		app->SaveAttributeToConfig(name, "vsync", "value", vSyncOnRestart ? "true" : "false");
	}

	int cameraSpeed = 1;
	if(app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT)
		cameraSpeed *= 10;
	if(app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT)
		cameraSpeed *= 100;
	if(app->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
		cameraSpeed *= 250;

	if(app->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT)
		camera.y += cameraSpeed;

	if(app->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
		camera.y -= cameraSpeed;

	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT)
		camera.x += cameraSpeed;

	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT)
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
	if(app->input->GetKey(SDL_SCANCODE_I) == KeyState::KEY_DOWN && fpsTarget < 1000)
	{
		fpsTarget += 10;
		ticksForNextFrame = 1000/fpsTarget;
	}
	if(app->input->GetKey(SDL_SCANCODE_K) == KeyState::KEY_DOWN && fpsTarget > 10)
	{
		fpsTarget -= 10;
		ticksForNextFrame = 1000/fpsTarget;
	}
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

bool Render::DrawCharacterTexture(SDL_Texture *texture, iPoint const &pos, const bool flip, SDL_Point pivot, const iPoint offset, const double angle, int flipValue) const
{
	SDL_Rect rect{0};

	SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);

	rect.x = pos.x + camera.x;
	rect.y = pos.y + camera.y;

	if(flip)
	{
		rect.x -= pivot.x;
	}

	if(offset != iPoint(INT_MAX, INT_MAX))
	{
		rect.x -= offset.x;
		rect.y -= offset.y;
	}

	SDL_Point const *p = nullptr;

	if(pivot.x != INT_MAX && pivot.y != INT_MAX)
	{
		SDL_Point sdlPivot{pivot.x, pivot.y};
		p = &sdlPivot;
	}
		
	if(SDL_RenderCopyEx(renderer.get(), texture, nullptr, &rect, angle, p, (SDL_RendererFlip)(flip + flipValue)) == -1)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		return false;
	}

	return true;
}

// Blit to screen
bool Render::DrawBackground(SDL_Texture *texture, fPoint pos, float scale) const
{
	iPoint position = {
		static_cast<int>(floor(pos.x)),
		static_cast<int>(floor(pos.y))
	};

	SDL_Rect rect = {
		.x = position.x,
		.y = position.y,
		.w = 0,
		.h = 0
	};

	SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
	
	rect.w = static_cast<int>(static_cast<float>(rect.w) * scale);
	rect.h = static_cast<int>(static_cast<float>(rect.h) * scale);

	if(SDL_RenderCopyEx(renderer.get(), texture, nullptr, &rect, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		return false;
	}

	return true;
}

// Blit to screen
bool Render::DrawImage(SDL_Texture *texture, iPoint position, float scale) const
{
	SDL_Rect rect = {
		.x = position.x,
		.y = position.y,
		.w = 0,
		.h = 0
	};

	SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);

	rect.w = static_cast<int>(static_cast<float>(rect.w) * scale);
	rect.h = static_cast<int>(static_cast<float>(rect.h) * scale);

	if(SDL_RenderCopyEx(renderer.get(), texture, nullptr, &rect, 0, nullptr, SDL_RendererFlip::SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		return false;
	}

	return true;
}

// Blit to screen
bool Render::DrawFont(SDL_Texture *texture, iPoint position, fPoint scale, const SDL_Rect *section, double angle, SDL_Point pivot) const
{
	SDL_Rect rect = {
		.x = position.x + camera.x,
		.y = position.y + camera.y,
		.w = 0,
		.h = 0
	};

	if(section)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);

	rect.w = static_cast<int>(static_cast<float>(rect.w) * scale.x);
	rect.h = static_cast<int>(static_cast<float>(rect.h) * scale.y);

	SDL_Point const *p = nullptr;

	if(pivot.x != INT_MAX && pivot.y != INT_MAX)
	{
		SDL_Point tempPivot{pivot.x, pivot.y};
		p = &tempPivot;
	}

	if(SDL_RenderCopyEx(renderer.get(), texture, section, &rect, angle, p, SDL_RendererFlip::SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		return false;
	}

	return true;
}

// Blit to screen
bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivotX, int pivotY, SDL_RendererFlip flip) const
{
	uint scale = app->win->GetScale();

	SDL_Rect rect = {
		.x = static_cast<int>((static_cast<float>(camera.x) * speed)) + x * static_cast<int>(scale),
		.y = static_cast<int>((static_cast<float>(camera.y) * speed)) + y * static_cast<int>(scale),
		.w = 0,
		.h = 0
	};

	if(section != nullptr)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point const *p = nullptr;

	if(pivotX != INT_MAX && pivotY != INT_MAX)
	{
		SDL_Point pivot{ pivotX, pivotY };
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer.get(), texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool Render::DrawRectangle(const SDL_Rect& rect, SDL_Color color, bool filled, bool use_camera, SDL_BlendMode blendMode) const
{
	SDL_SetRenderDrawBlendMode(renderer.get(), blendMode);
	SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);

	SDL_Rect rec(rect);
	
	if(use_camera)
	{	
		uint scale = app->win->GetScale();
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

bool Render::DrawLine(iPoint v1, iPoint v2, SDL_Color color, bool use_camera, SDL_BlendMode blendMode) const
{

	SDL_SetRenderDrawBlendMode(renderer.get(), blendMode);
	SDL_SetRenderDrawColor(renderer.get(), color.r, color.g, color.b, color.a);
	
	uint scale = app->win->GetScale();
	
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

bool Render::DrawCircle(iPoint center, int radius, SDL_Color color, bool use_camera, SDL_BlendMode blendMode) const
{
	[[maybe_unused]] uint scale = app->win->GetScale();

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
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	vSyncOnRestart = data.child("graphics").attribute("vsync").as_bool();
	fpsTarget = data.child("graphics").attribute("targetfps").as_uint();

	return true;
}

pugi::xml_node Render::SaveState(pugi::xml_node const &data) const
{
	pugi::xml_node cam = data;
	cam = cam.append_child("renderer");

	cam.append_child("camera").append_attribute("x").set_value(camera.x);
	cam.child("camera").append_attribute("y").set_value(camera.y);
	
	cam.append_child("graphics").append_attribute("vsync").set_value(vSyncOnRestart ? "true" : "false");
	cam.child("graphics").append_attribute("targetfps").set_value(std::to_string(fpsTarget).c_str());

	return cam;
}

bool Render::HasSaveData() const
{
	return true;
}

std::shared_ptr<SDL_Texture> Render::LoadTexture(SDL_Surface *surface)
{
	std::shared_ptr<SDL_Texture>texturePtr(
			SDL_CreateTextureFromSurface(renderer.get(), surface),
			[](SDL_Texture *tex) { if(tex) SDL_DestroyTexture(tex); }
	);
	return texturePtr;
}

SDL_Rect Render::GetCamera() const
{
	return camera;
}

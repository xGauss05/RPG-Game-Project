#ifndef __RENDER_H__
#define __RENDER_H__

#include "Module.h"
#include "Defs.h"
#include "Point.h"

#include <memory>
#include <functional>

#include "SDL/include/SDL.h"

struct DrawParameters
{
	int textureID;
	iPoint position;
	const SDL_Rect* section = nullptr;
	fPoint parallaxSpeed = { 1.0f, 1.0f };
	double rotationAngle = 0;
	SDL_Point center = SDL_Point(INT_MAX, INT_MAX);
	iPoint rectOffset = iPoint(INT_MAX, INT_MAX);
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	fPoint scale = { 0.0f, 0.0f };

	DrawParameters(int tex, iPoint pos)
		: textureID(tex), position(pos) {}

	DrawParameters& Section(const SDL_Rect* s)
	{
		section = s;
		return *this;
	}
	DrawParameters& ParallaxSpeed(fPoint p)
	{
		parallaxSpeed = p;
		return *this;
	}
	DrawParameters& RotationAngle(double a)
	{
		rotationAngle = a;
		return *this;
	}
	DrawParameters& Center(SDL_Point p)
	{
		center = p;
		return *this;
	}
	DrawParameters& RectOffset(iPoint p)
	{
		rectOffset = p;
		return *this;
	}
	DrawParameters& Flip(SDL_RendererFlip f)
	{
		flip = f;
		return *this;
	}
	DrawParameters& Scale(fPoint s)
	{
		scale = s;
		return *this;
	}
};

class Render : public Module
{
public:

	Render();

	// Destructor
	~Render() final;

	// Called before render is available
	bool Awake(pugi::xml_node &) final;

	// Called before the first frame
	bool Start() final;

	// Called each loop iteration
	bool PreUpdate() final;
	bool Update(float dt) final;
	bool PostUpdate() final;

	bool Pause(int phase) final;

	// Called before quitting
	bool CleanUp() final;

	bool DrawTexture(DrawParameters const& params) const;

	bool DrawShape(
		const SDL_Rect &rect,
		bool filled,
		SDL_Color color,
		bool useCamera = true,
		SDL_BlendMode blendMode = SDL_BlendMode::SDL_BLENDMODE_BLEND
	) const;

	bool DrawShape(
		iPoint v1,
		iPoint v2,
		SDL_Color color,
		bool use_camera = true,
		SDL_BlendMode blendMode = SDL_BlendMode::SDL_BLENDMODE_BLEND
	) const;

	bool DrawShape(
		iPoint center,
		int radius,
		SDL_Color color,
		bool use_camera = true,
		SDL_BlendMode blendMode = SDL_BlendMode::SDL_BLENDMODE_BLEND
	) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

	bool LoadState(pugi::xml_node const &) final;
	pugi::xml_node SaveState(pugi::xml_node const &) const final;

	bool HasSaveData() const final;

	SDL_Rect GetCamera() const;

	SDL_Renderer* GetRender() const;

private:

	void SetViewPort(const SDL_Rect &rect) const;
	void ResetViewPort() const;


	std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>> renderer;
	SDL_Rect viewport;
	SDL_Color background;
	SDL_Rect camera;

	// -------- Vsync
	bool vSyncActive = true;
	bool vSyncOnRestart = true;

	// -------- No Vsync
	// Max fps we want to achieve
	uint32 fpsTarget = 60;
	
	// -------- Required for capping FPS
	// Delay required in ms to get the fps target
	uint32 ticksForNextFrame = 0;
	// Last tick in which we updated render
	uint32 renderLastTime = 0;
	// Remember last fps for the 30fps toggle option
	uint32 prevFPSTarget = 0;
	
	// -------- Required for showing FPS on screen
	// FPS on last update
	uint32 fps = 0;
	// Frames since last tick
	uint32 fpsCounter = 0;
	// Last tick in which we updated the current fps
	uint32 fpsLast = 0;
};

#endif // __RENDER_H__

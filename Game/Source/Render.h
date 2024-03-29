#ifndef __RENDER_H__
#define __RENDER_H__

#include "Module.h"
#include "Defs.h"
#include "Point.h"
#include "Easing.h"

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
	bool gradient = false;

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
	DrawParameters& Gradient(bool b)
	{
		gradient = b;
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

	void DrawGradientBar(
		SDL_Rect const& rect,
		SDL_Color start,
		SDL_Color end,
		uint8_t numberOfColors = 5
	) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

	bool LoadState(pugi::xml_node const &) final;
	pugi::xml_node SaveState(pugi::xml_node const &) const final;

	bool HasSaveData() const final;

	void ToggleVSync();

	SDL_Rect GetCamera() const;
	void AdjustCamera(iPoint position);
	void ResetCamera();

	SDL_Renderer* GetRender() const;

	void StartEasing(std::string_view name);
	int AddEasing(float totalTime);
	bool DrawEasing(int textureID, std::string_view name);
	bool EasingHasFinished(std::string_view name);

	void InitEasings(pugi::xml_node const& node);

private:
	
	void SetViewPort(const SDL_Rect &rect) const;
	void ResetViewPort() const;

	std::vector<Easing> easings;

	std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>> renderer;
	SDL_Rect viewport;
	SDL_Color background;
	SDL_Rect camera;

	// -------- Vsync
	bool vSyncActive = false;
	bool vSyncOnRestart = true;
};

#endif // __RENDER_H__

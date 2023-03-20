#include "Window.h"
#include "App.h"

#include "Defs.h"
#include "Log.h"

#include <string>
#include <memory>
#include <functional>

#include "SDL/include/SDL.h"


Window::Window() : Module()
{
	name = "window";
}

// Destructor
Window::~Window() = default;

// Called before render is available
bool Window::Awake(pugi::xml_node& config)
{
	LOG("Init SDL window & surface");

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// Read config.xml
	width = config.child("resolution").attribute("width").as_int();
	height = config.child("resolution").attribute("height").as_int();
	scale = config.child("resolution").attribute("scale").as_float(); 

	bool fullscreen = config.child("fullscreen").attribute("value").as_bool();
	bool borderless = config.child("bordeless").attribute("value").as_bool();
	bool resizable = config.child("resizable").attribute("value").as_bool();
	bool fullscreen_window = config.child("fullscreen_window").attribute("value").as_bool();
		
	// Set Window flags
	Uint32 flags = SDL_WINDOW_SHOWN;
	if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (borderless) flags |= SDL_WINDOW_BORDERLESS;
	if (resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (fullscreen_window) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	// Create Window
	std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>windowPtr(
		SDL_CreateWindow(
			app->GetTitle().c_str(),
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			flags
		),
		[](SDL_Window *win) { if(win) SDL_DestroyWindow(win); }
	);

	window = std::move(windowPtr);
	
	if (!window)
	{
		LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	// Get window surface
	std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface *)>>surfacePtr(
		SDL_GetWindowSurface(window.get()),
		[](SDL_Surface *surface) { if(surface) SDL_FreeSurface(surface);  }
	);

	screenSurface = std::move(surfacePtr);

	return true;
}

// Called before quitting
bool Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	// Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void Window::SetTitle(std::string const &newTitle)
{
	SDL_SetWindowTitle(window.get(), newTitle.c_str());
}

SDL_Window *Window::GetWindow() const
{
	return window.get();
}

SDL_Surface *Window::GetSurface() const
{
	return screenSurface.get();
}

void Window::GetWindowSize(uint &w, uint &h) const
{
	w = width;
	h = height;
}

iPoint Window::GetWindowSize() const
{
	return iPoint(width, height);
}

float Window::GetScale() const
{
	return scale;
}

int Window::GetHeight() const
{
	return height;
}

int Window::GetWidth() const
{
	return width;
}

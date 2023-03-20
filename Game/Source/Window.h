#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"
#include "Defs.h"
#include "Point.h"

#include <memory>
#include <functional>

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();

	// Destructor
	~Window() final;

	// Called before render is available
	bool Awake(pugi::xml_node&) final;

	// Called before quitting
	bool CleanUp() final;

	// Change title
	void SetTitle(std::string const &title);

	SDL_Window *GetWindow() const;

	SDL_Surface *GetSurface() const;

	// Retrieve window size
	void GetWindowSize(uint &w, uint &h) const;
	iPoint GetWindowSize() const;
	// Retrieve window scale
	float GetScale() const;
	int GetHeight() const; 
	int GetWidth() const; 

private:
	// The window we'll be rendering to
	std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> window;

	// The surface contained by the window
	std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface *)>> screenSurface;

	std::string title = "";
	uint width;
	uint height;
	float scale;
};

#endif // __WINDOW_H__

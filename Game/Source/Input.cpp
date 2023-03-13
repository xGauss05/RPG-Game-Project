#include "App.h"
#include "Input.h"
#include "Window.h"

#include "Point.h"
#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL.h"



Input::Input() : Module()
{
	name = "input";
}

// Destructor
Input::~Input() = default;

// Called before render is available
bool Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

// Called before the first frame
bool Input::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	static SDL_Event event;

	const Uint8* keys = SDL_GetKeyboardState(nullptr);

	using enum EventWindow;
	using enum KeyState;

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE || keyboard[i] == KEY_UP)
				keyboard[i] = KEY_DOWN;
			else keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_DOWN || keyboard[i] == KEY_REPEAT)
				keyboard[i] = KEY_UP;
			else keyboard[i] = KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouseButtons[i] == KEY_DOWN)
			mouseButtons[i] = KEY_REPEAT;

		if(mouseButtons[i] == KEY_UP)
			mouseButtons[i] = KEY_IDLE;
	}

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[static_cast<uint>(EventWindow::WE_QUIT)] = true;
			break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[static_cast<uint>(EventWindow::WE_HIDE)] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
					windowEvents[static_cast<uint>(EventWindow::WE_SHOW)] = true;
					break;

					default:
						break;
				}
			break;

			case SDL_MOUSEBUTTONDOWN:
				mouseButtons[event.button.button - 1] = KeyState::KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
			break;
			
			case SDL_MOUSEBUTTONUP:
				mouseButtons[event.button.button - 1] = KeyState::KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
			break;

			case SDL_MOUSEMOTION:
			{
				int scale = app->win->GetScale();
				mouseMotion.x = event.motion.xrel / scale;
				mouseMotion.y = event.motion.yrel / scale;
				mousePosition.x = event.motion.x / scale;
				mousePosition.y = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;
			}
			default:
				break;
		}
	}

	return true;
}

// Called when game is paused
bool Input::Pause(int phase)
{
	return PreUpdate();
}

// Called before quitting
bool Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}


bool Input::GetWindowEvent(EventWindow ev)
{
	return windowEvents[static_cast<uint>(ev)];
}

iPoint Input::GetMousePosition() const
{
	return mousePosition;
}

uPoint Input::GetUnsignedMousePosition() const
{
	return { (uint)mousePosition.x, (uint)mousePosition.y };
}

void Input::GetMousePosition(int& x, int& y) const
{
	x = mousePosition.x;
	y = mousePosition.y;
}

void Input::GetMouseMotion(int& x, int& y) const
{
	x = mouseMotion.x;
	y = mouseMotion.y;
}
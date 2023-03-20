#ifndef __INPUT_H__
#define __INPUT_H__

#include "Module.h"
#include "Point.h"

#include <array>

//#define NUM_KEYS 352
constexpr auto NUM_MOUSE_BUTTONS = 5;
constexpr auto NUM_EVENT_WINDOW = 4;
constexpr auto MAX_KEYS = 300;
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;

enum class EventWindow : uint
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT = 3
};

enum class KeyState : uint
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class Input : public Module
{

public:

	Input();

	// Destructor
	~Input() final;

	// Called before render is available
	bool Awake(pugi::xml_node&) final;

	// Called before the first frame
	bool Start() final;

	// Called each loop iteration
	bool PreUpdate() final;

	bool Pause(int phase) final;

	// Called before quitting
	bool CleanUp() final;

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(uint id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButtonDown(uint id) const
	{
		return mouseButtons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(EventWindow ev);


	// Get mouse / axis position
	iPoint GetMousePosition() const;
	uPoint GetUnsignedMousePosition() const;
	void GetMousePosition(int &x, int &y) const;
	void GetMouseMotion(int& x, int& y) const;

private:
	std::array<bool, NUM_EVENT_WINDOW> windowEvents{};
	std::array<KeyState, MAX_KEYS> keyboard{};
	std::array<KeyState, NUM_MOUSE_BUTTONS> mouseButtons{};
	iPoint mouseMotion;
	iPoint mousePosition;
};

#endif // __INPUT_H__
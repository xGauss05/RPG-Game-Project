#include "Quest_Collect.h"

#include "Input.h"
#include "Log.h"

#include <format>
#include <string>


bool Quest_Collect::Update()
{
	if (app->input->GetKey(SDL_SCANCODE_Y) == KeyState::KEY_DOWN)
	{
		return false;
	}

	return true;
}
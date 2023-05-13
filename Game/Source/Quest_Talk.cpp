#include "Quest_Talk.h"

#include "Input.h"
#include "Log.h"

#include <format>
#include <string>


bool Quest_Talk::Update()
{
	if (app->input->GetKey(SDL_SCANCODE_U) == KeyState::KEY_DOWN)
	{
		return false;
	}

	return true;
}
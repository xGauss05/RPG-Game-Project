#include "GuiButton.h"
#include "App.h"
#include "Render.h"
#include "Input.h"
#include "TextManager.h"
#include "Audio.h"
#include "Log.h"

#include <map>

GuiButton::~GuiButton()
{
	app->tex->Unload(textureID);
}

GuiButton::GuiButton(uPoint startingPos, uPoint targetPos, uPoint size, std::string const& str, std::function<int()> const& funcPtr, std::vector<SDL_Rect> const& buttonStates) :
	text(str),
	currentState(ButtonState::NORMAL)
{
	Initialize(funcPtr, startingPos,targetPos, size);

	textureID = app->tex->Load("Assets/UI/GUI_4x_sliced.png");
	pressedFx = app->audio->LoadFx("Assets/Audio/Fx/S_Menu-Pressed.wav");
	focusedFx = app->audio->LoadFx("Assets/Audio/Fx/S_Menu-Focused.wav");

	for (int i = 0; auto const& elem : buttonStates)
	{
		panels.try_emplace(i, elem, 4, textureID, iPoint(3, 3));
		i++;
	}
}

int GuiButton::Update()
{
	using enum ButtonState;

	if (currentState == DISABLED || !IsEnabled())
		return 0;

	if (IsMouseHovering() && app->input->controllerCount <= 0)
	{
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
		{
			if (currentState != PRESSED) 
			{
				currentState = PRESSED;
				playedSound = false;
			}

		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
		{
			currentState = NORMAL;
			return ExecuteFunction();
		}
		else if (currentState != FOCUSED)
		{
			currentState = FOCUSED;
			playedSound = false;

		}
	}
	else if (IsControllerHovered())
	{
		if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_A) == KeyState::KEY_REPEAT)
		{
			currentState = PRESSED;
			playedSound = false;

		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_A) == KeyState::KEY_UP)
		{
			currentState = NORMAL;
			return ExecuteFunction();
		}
		else if (currentState != FOCUSED)
		{
			currentState = FOCUSED;
			playedSound = false;
		}
	}
	else
	{
		currentState = NORMAL;
		playedSound = true;
	}

	if (!playedSound)
	{
		switch (currentState)
		{
		case NORMAL:
			break;
		case PRESSED:
			playedSound = true;
			app->audio->PlayFx(pressedFx);
			break;
		case FOCUSED:
			playedSound = true;
			app->audio->PlayFx(focusedFx);
			break;
		case DISABLED:
			break;
		default:
			break;
		}
	}

	return 0;
}

bool GuiButton::Draw() const
{
	auto centerPoint = iPoint(GetCurrentPosition().x, GetCurrentPosition().y);

	if (auto result = panels.find(static_cast<int>(currentState));
		result != panels.end())
	{
		result->second.Draw(centerPoint, iPoint(GetSize().x, GetSize().y));
	}

	centerPoint += iPoint(GetSize().x / 2, GetSize().y / 2);

	app->fonts->DrawText(text, TextParameters(0, DrawParameters(0, centerPoint)).Align(AlignTo::ALIGN_CENTER));

	return true;
}

void GuiButton::MouseEnterHandler()
{
	if (currentState != ButtonState::DISABLED)
		currentState = ButtonState::FOCUSED;
}

void GuiButton::MouseLeaveHandler()
{
	if (currentState != ButtonState::DISABLED)
		currentState = ButtonState::NORMAL;
}

void GuiButton::ToggleSelected()
{
	using enum ButtonState;
	currentState = (currentState == SELECTED)
		? NORMAL
		: SELECTED;
}

void GuiButton::DebugDraw() const
{
	SDL_Rect debugRect(GetCurrentPosition().x, GetCurrentPosition().y, GetSize().x, GetSize().y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

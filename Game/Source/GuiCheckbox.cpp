#include "GuiCheckbox.h"
#include "App.h"
#include "Render.h"
#include "Input.h"
#include "TextManager.h"
#include "Audio.h"
#include "Log.h"

#include <map>

GuiCheckbox::~GuiCheckbox()
{
	app->tex->Unload(textureID);
}

GuiCheckbox::GuiCheckbox(uPoint startingPos, uPoint targetPos, uPoint size, std::string const& str, std::function<int()> const& funcPtr, std::vector<SDL_Rect> const& buttonStates) :
	text(str),
	currentState(CheckboxState::UNSELECTED)
{
	Initialize(funcPtr, startingPos, targetPos, size);

	textureID = app->tex->Load("Assets/UI/GUI_4x_sliced.png");
	pressedFx = app->audio->LoadFx("Fx/S_Menu-Pressed.wav");
	focusedFx = app->audio->LoadFx("Fx/S_Menu-Focused.wav");

	for (int i = 0; auto const& elem : buttonStates)
	{
		panels.try_emplace(i, elem, 4, textureID, iPoint(3, 3));
		i++;
	}
}

int GuiCheckbox::Update()
{
	using enum CheckboxState;
	if (currentState == DISABLED)
		return 0;

	if (IsMouseHovering() && app->input->controllerCount <= 0)
	{
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
		{
			if (currentState == CheckboxState::SELECTED || currentState == CheckboxState::SELECTED_FOCUSED)
				currentState = CheckboxState::UNSELECTED;
			else if (currentState == CheckboxState::UNSELECTED || currentState == CheckboxState::UNSELECTED_FOCUSED)
				currentState = CheckboxState::SELECTED;

			playedSound = false;

			return ExecuteFunction();
		}
		else if (currentState != CheckboxState::SELECTED_FOCUSED && currentState != CheckboxState::UNSELECTED_FOCUSED)
		{
			if (currentState == CheckboxState::SELECTED)
			{
				playedSound = false;

				currentState = CheckboxState::SELECTED_FOCUSED;
			}

			if (currentState == CheckboxState::UNSELECTED)
			{
				playedSound = false;

				currentState = CheckboxState::UNSELECTED_FOCUSED;
			}
		}
	}
	else if (IsControllerHovered())
	{
		if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_A) == KeyState::KEY_UP)
		{
			if (currentState == CheckboxState::SELECTED || currentState == CheckboxState::SELECTED_FOCUSED)
				currentState = CheckboxState::UNSELECTED;
			else if (currentState == CheckboxState::UNSELECTED || currentState == CheckboxState::UNSELECTED_FOCUSED)
				currentState = CheckboxState::SELECTED;

			playedSound = false;

			return ExecuteFunction();
		}
		else if (currentState != CheckboxState::SELECTED_FOCUSED && currentState != CheckboxState::UNSELECTED_FOCUSED)
		{
			if (currentState == CheckboxState::SELECTED)
			{
				playedSound = false;

				currentState = CheckboxState::SELECTED_FOCUSED;
			}

			if (currentState == CheckboxState::UNSELECTED)
			{
				playedSound = false;

				currentState = CheckboxState::UNSELECTED_FOCUSED;
			}
		}
	}
	else
	{
		if (currentState == CheckboxState::SELECTED_FOCUSED) currentState = CheckboxState::SELECTED;

		if (currentState == CheckboxState::UNSELECTED_FOCUSED) currentState = CheckboxState::UNSELECTED;

		playedSound = true;

	}

	if (!playedSound)
	{
		switch (currentState)
		{
		case SELECTED:
		case UNSELECTED:
			playedSound = true;
			app->audio->PlayFx(pressedFx);
			break;
		case SELECTED_FOCUSED:
		case UNSELECTED_FOCUSED:
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

bool GuiCheckbox::Draw() const
{
	auto centerPoint = iPoint(GetCurrentPosition().x, GetCurrentPosition().y);

	if (auto result = panels.find(static_cast<int>(currentState));
		result != panels.end())
	{
		result->second.Draw(centerPoint, iPoint(GetSize().x, GetSize().y));
	}

	centerPoint += iPoint(GetSize().x / 2, GetSize().y / 2);

	return true;
}

void GuiCheckbox::MouseEnterHandler()
{
	if (currentState != CheckboxState::DISABLED)
		currentState = CheckboxState::SELECTED_FOCUSED;
}

void GuiCheckbox::MouseLeaveHandler()
{
	if (currentState != CheckboxState::DISABLED)
		currentState = CheckboxState::UNSELECTED;
}

void GuiCheckbox::DebugDraw() const
{
	SDL_Rect debugRect(GetCurrentPosition().x, GetCurrentPosition().y, GetSize().x, GetSize().y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

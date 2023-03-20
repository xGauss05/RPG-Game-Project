#include "GuiButton.h"
#include "App.h"
#include "Render.h"
#include "Input.h"
#include "TextManager.h"

#include "Log.h"

#include <map>

GuiButton::~GuiButton()
{
	for (auto const &[index, panel] : panels)
	{
		panel.Unload();
	}
}

GuiButton::GuiButton(uPoint pos, uPoint size, std::string const &str, std::function<int()> const& funcPtr, std::vector<SDL_Rect> const &buttonStates) :
	text(str),
	currentState(ButtonState::NORMAL)
{
	Initialize(funcPtr, pos, size);

	int textureID = app->tex->Load("Assets/UI/GUI_4x_sliced.png");

	for (int i = 0; auto const& elem : buttonStates)
	{
		panels.try_emplace(i, elem, 4, textureID, iPoint(3, 3));
		i++;
	}
}

int GuiButton::Update()
{
	using enum ButtonState;
	if (currentState == DISABLED)
		return 0;

	if (IsMouseHovering())
	{
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
		{
			currentState = PRESSED;
		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
		{
			return ExecuteFunction();
			currentState = NORMAL;
		}
		else
		{
			currentState = FOCUSED;
		}
	}
	else
	{
		currentState = NORMAL;
	}
	return 0;
}

bool GuiButton::Draw() const
{
	auto centerPoint = iPoint(GetPosition().x, GetPosition().y);

	if (auto result = panels.find(static_cast<int>(currentState));
		result != panels.end())
	{
		result->second.Draw(centerPoint, iPoint(GetSize().x, GetSize().y));
	}

	centerPoint += iPoint(GetSize().x/2, GetSize().y/2);

	TextParameters params(0, DrawParameters(0, centerPoint));
	params.align = AlignTo::ALIGN_CENTER;

	app->fonts->DrawText(
		text,
		params
	);

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

void GuiButton::DebugDraw() const
{
	SDL_Rect debugRect(GetPosition().x, GetPosition().y, GetSize().x, GetSize().y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

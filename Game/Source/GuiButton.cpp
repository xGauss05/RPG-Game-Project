#include "GuiButton.h"
#include "App.h"
#include "Input.h"

GuiButton::GuiButton(uPoint pos, uPoint size, std::string const &str, std::function<void()> const& funcPtr) :
	text(str),
	currentState(ButtonState::NORMAL)
{
	Initialize(funcPtr, pos, size);
}

void GuiButton::Update()
{
	if (currentState == ButtonState::DISABLED)
		return;

	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
		currentState = ButtonState::PRESSED;

	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
		ExecuteFunction();
}

bool GuiButton::Draw()
{
	return false;
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

#pragma once

#include "GuiElement.h"
#include "Textures.h"

#include <memory>
#include <string>

enum class ButtonState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED
};

class GuiButton : public GuiElement
{
public:
	GuiButton() = default;
	~GuiButton() override = default;
	explicit GuiButton(
		uPoint pos,
		uPoint size,
		std::string const &str,
		std::function<void()> const& funcPtr
	);

	void Update() override;
	bool Draw() override;

	void MouseEnterHandler() override;
	void MouseLeaveHandler() override;

private:
    std::string text = "";
    std::shared_ptr<SDL_Texture> texture = nullptr;
	ButtonState currentState = ButtonState::DISABLED;
};


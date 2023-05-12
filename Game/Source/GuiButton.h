#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "GuiElement.h"
#include "GuiPanelSegmented.h"
#include "TextureManager.h"

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
	~GuiButton() override;


	explicit GuiButton(
		uPoint startingPos,
		uPoint targetPos,
		uPoint size,
		std::string const &str,
		std::function<int()> const& funcPtr,
		std::vector<SDL_Rect> const& buttonStates
	);

	int Update() override;
	bool Draw() const override;

	void MouseEnterHandler() override;
	void MouseLeaveHandler() override;

	void ToggleSelected();

	void DebugDraw() const;

private:
    std::string text = "";
	int font = 0;
	int pressedFx = 0;
	int focusedFx = 0;

	int textureID = 0;

	bool playedSound = true;

	std::map<int, GuiPanelSegmented> panels;

	iPoint offset;

	ButtonState currentState = ButtonState::DISABLED;
	ButtonState lastState = ButtonState::DISABLED;
};

#endif __GUIBUTTON_H__
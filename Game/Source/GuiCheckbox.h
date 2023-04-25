#ifndef __GUICHECKBOX_H__
#define __GUICHECKBOX_H__

#include "GuiElement.h"
#include "GuiPanelSegmented.h"
#include "TextureManager.h"

#include <memory>
#include <string>


enum class CheckboxState
{
	DISABLED,
	NORMAL,
	PRESSED,
	FOCUSED,
	SELECTED
};

class GuiCheckbox : public GuiElement
{
public:
	GuiCheckbox() = default;
	~GuiCheckbox() override;

	explicit GuiCheckbox(
		uPoint pos,
		uPoint size,
		std::string const& str,
		std::function<int()> const& funcPtr,
		std::vector<SDL_Rect> const& buttonStates
	);

	int Update() override;
	bool Draw() const override;

	void MouseEnterHandler() override;
	void MouseLeaveHandler() override;

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

	CheckboxState currentState = CheckboxState::DISABLED;

};

#endif __GUICHECKBOX_H__
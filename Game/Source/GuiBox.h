#ifndef __GUIBOX_H__
#define __GUIBOX_H__

#include "GuiElement.h"
#include "GuiPanelSegmented.h"
#include "TextureManager.h"

#include <memory>
#include <string>

class GuiBox : public GuiElement
{
public:
	GuiBox() = default;
	~GuiBox() override;

	explicit GuiBox(
		uPoint pos,
		uPoint widthHeight,
		SDL_Rect const &rect,
		int advance,
		int id,
		iPoint tSegments,
		std::string const &t = ""
	);

	bool Draw() const override;
	std::string_view GetText();
	void ModifyText(std::string_view newText);
	void DebugDraw() const;

private:
	std::unique_ptr<GuiPanelSegmented> panel;

	iPoint offset;
	std::string text = "";
};

#endif //__GUIBOX_H__
#ifndef __GUIBOX_H__
#define __GUIBOX_H__

#include "GuiElement.h"
#include "GuiPanelSegmented.h"
#include "TextureManager.h"

#include <memory>
#include <string>

class GuiMenuList : public GuiElement
{
public:
	GuiMenuList() = default;
	~GuiMenuList() override;

	explicit GuiMenuList(
		uPoint pos, 
		uPoint widthHeight,
		SDL_Rect const &rect,
		int advance,
		iPoint tSegments
	);

	bool Draw() const override;
	void DebugDraw() const;

private:
	std::unique_ptr<GuiPanelSegmented> panel;
};

#endif //__GUIBOX_H__
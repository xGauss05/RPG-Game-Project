#ifndef __WINDOW_MENU_LIST_H__
#define __WINDOW_MENU_LIST_H__

#include "GuiElement.h"
#include "GuiPanelSegmented.h"
#include "TextureManager.h"
#include "TextManager.h"

#include <memory>
#include <string>

class GuiMenuList
{
public:
	class MenuItem
	{
	public:
		struct ItemText
		{
			std::string leftText = "";
			std::string centerText = "";
			std::string rightText = "";
		};

		MenuItem() = default;
		explicit MenuItem(ItemText const& itemText, int textureID = -1);

		void Draw(iPoint originalPos, iPoint rectSize, iPoint innerMargin = iPoint(0, 0), int iconSize = 0) const;
		void DebugDraw(iPoint pos, iPoint s, int outterMarginY, int menuItemHeight, int index, int scroll) const;

	private:

		ItemText text;
		int iconTexture = -1;
	};

	GuiMenuList() = default;
	explicit GuiMenuList(pugi::xml_node const& node);
	virtual ~GuiMenuList();

	void Update();

	bool Draw() const;
	void DebugDraw() const;

protected:
	virtual void HandleLeftButtonClick(int result);
	virtual void HandleRightButtonClick();

private:
	void HandleInput();
	void HandleLeftClick();
	void HandleWheelScroll();
	void SelectAndScrollUpIfNeeded(int amount = 1);
	void SelectAndScrollDownIfNeeded(int amount = 1);
	void ScrollListUp(int amount = 1);
	void ScrollListDown(int amount = 1);

	std::unique_ptr<GuiPanelSegmented> background;
	SDL_Rect arrowRect{ 0 };

	iPoint position{ 0 };
	iPoint size{ 0 };
	iPoint outterMargin{ 16, 16 };

	int menuItemHeight = 96;
	int maxColumns = 1;
	int maxElements = -1;	//Max number of elements that will be shown. -1 for fit to size
	iPoint itemMargin{ 2, 2 };

	int currentItemSelected = 0;
	int currentScroll = 0;
	int lastTimeSinceScrolled = 0;

	int iconSize = 0;

	int fontID = 0;

	std::vector<MenuItem> items;
};

#endif //__WINDOW_MENU_LIST_H__
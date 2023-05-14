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
		explicit MenuItem(ItemText const& itemText, int i, int textureID = -1);

		void Draw(
			iPoint originalPos,
			iPoint rectSize,
			iPoint innerMargin = iPoint(0, 0),
			iPoint outMargin = iPoint(0, 0),
			Uint8 animationAlpha = 0,
			int iconSize = 0,
			bool currentlySelected = false
		) const;

		void DebugDraw(iPoint pos, iPoint s, int outterMarginY, int itemHeight, int index, int scroll) const;

		void SetText(ItemText const &newText);
		void SetText(int align, std::string_view newText);

	private:

		ItemText text;
		int index = -1;
		int iconTexture = -1;
	};

	GuiMenuList();
	explicit GuiMenuList(pugi::xml_node const& node);
	virtual ~GuiMenuList();

	void Initialize();

	void Start();

	bool Update();

	void CreateMenuItem(MenuItem const& item);
	void DeleteMenuItem(int index);

	bool Draw() const;
	void DebugDraw() const;

	bool GoToPreviousMenu() const;
	bool GetClickHandled() const;
	int GetLastClick() const;
	bool GetGoToPreviousMenu() const;

protected:
	virtual void HandleLeftButtonClick(int result) = 0;
	virtual void HandleRightButtonClick() = 0;
	virtual void InitializeElements() = 0;

	void SetDeleteMenu(bool b);
	void SetClickHandled(bool b);
	void SetGoToPreviousMenu(bool b);

	void SetCurrentAlpha(Uint8 value);
	void SetCurrentItemSelected(int value);

	void ResetCurrentItemSelected();

private:
	void HandleInput();
	void HandleLeftClick();
	void HandleWheelScroll();
	void SelectAndScrollUpIfNeeded(int amount = 1);
	void SelectAndScrollDownIfNeeded(int amount = 1);
	void ScrollListUp(int amount = 1);
	void ScrollListDown(int amount = 1);

	void SetLastClick(int i = -1);

	void UpdateAlpha();

	void SetDefaultBooleanValues();

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

	Uint8 currentAlpha = 0;
	bool alphaIncreasing = true;

	bool deleteMenu = false;
	bool goToPreviousMenu = false;

	bool clickHandled = false;
	int lastClick = -1;

	std::vector<MenuItem> items;
};

#endif //__WINDOW_MENU_LIST_H__
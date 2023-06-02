#ifndef __WINDOW_MENU_LIST_H__
#define __WINDOW_MENU_LIST_H__

#include "TextureManager.h"
#include "TextManager.h"

#include "GuiSegmentedPanel.h"
#include "GuiMenuListItem.h"

#include <memory>
#include <string>

class GuiMenuList
{
public:
	GuiMenuList();
	explicit GuiMenuList(pugi::xml_node const& node);
	virtual ~GuiMenuList();

	void Initialize();

	virtual void Start();

	virtual bool Update();

	bool Draw() const;

	bool GoToPreviousMenu() const;
	bool GetClickHandled() const;
	int GetLastClick() const;
	void SetPreviousPanelLastClick(int n);
	bool GetGoToPreviousMenu() const;
	bool GetAndDefaultCloseAllMenus();

	void SetActive(bool n);

	void ToggleActive();
	bool IsActive() const;

	std::size_t GetNumberOfItems() const;

protected:
	virtual void HandleLeftButtonClick(int result) = 0;
	virtual void HandleRightButtonClick() = 0;
	virtual void InitializeElements() = 0;

	void SetDeleteMenu(bool b);
	void SetClickHandled(bool b);
	void SetGoToPreviousMenu(bool b);

	void SetCurrentAlpha(uint8_t value);
	void SetCurrentItemSelected(int value);

	void ResetCurrentItemSelected();

	void ClearMenuItems();
	int GetPreviousPanelLastClick() const;
	virtual void SetDefaultBooleanValues();

	void CreateMenuCharacter(Battler const& battler);
	void CreateMenuItem(std::string_view left, std::string_view center, std::string_view right, int textureID = -1);
	void CreateMenuImage(int textureId, SDL_Rect const& srcRect);
	void DeleteMenuItem(int index);

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

	GuiSegmentedPanel background;
	SDL_Rect arrowRect{ 0 };

	iPoint position{ 0 };
	iPoint size{ 0 };
	iPoint outterMargin{ 16, 16 };

	iPoint m_itemSize = { 96, 48 };
	int maxColumns = 1;
	int maxElements = -1;	//Max number of elements that will be shown. -1 for fit to size
	iPoint innerMargin{ 2, 2 };

	int currentItemSelected = 0;
	int currentScroll = 0;
	int lastTimeSinceScrolled = 0;

	int iconSize = 0;

	int fontID = 0;

	bool active = true;

	uint8_t currentAlpha = 0;
	int alphaDirection = 1;

	bool closeAllMenus = false;
	bool deleteMenu = false;
	bool goToPreviousMenu = false;

	bool clickHandled = false;
	int lastClick = -1;
	int previousPanelLastClick = -1;

	std::vector<std::unique_ptr<MenuItem>> items;
};

#endif //__WINDOW_MENU_LIST_H__
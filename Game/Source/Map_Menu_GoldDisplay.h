#ifndef __MAP_MENU_GOLDDISPLAY_H__
#define __MAP_MENU_GOLDDISPLAY_H__

#include "GuiMenuList.h"
#include "Map_Menu_ComponentParty.h"

class Map_Menu_GoldDisplay : public GuiMenuList, public Map_Menu_ComponentParty
{
public:
	Map_Menu_GoldDisplay() = default;
	explicit Map_Menu_GoldDisplay(pugi::xml_node const& node);
	~Map_Menu_GoldDisplay() override = default;

private:
	void HandleLeftButtonClick(int result) override;
	void HandleRightButtonClick() override;
	void InitializeElements() override;
};

#endif //__MAP_MENU_GOLDDISPLAY_H__

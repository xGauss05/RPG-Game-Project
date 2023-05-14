#ifndef __MAP_MENU_INVENTORY_H__
#define __MAP_MENU_INVENTORY_H__

#include "GuiMenuList.h"

class Map_Menu_Inventory : public GuiMenuList
{
public:
	Map_Menu_Inventory() = default;
	explicit Map_Menu_Inventory(pugi::xml_node const& node);
	~Map_Menu_Inventory() override = default;

private:
	void HandleLeftButtonClick(int result) override;
	void HandleRightButtonClick() override;
	void InitializeElements() override;

};

#endif //__MAP_MENU_INVENTORY_H__

#ifndef __MAP_MENU_INVENTORY_H__
#define __MAP_MENU_INVENTORY_H__

#include "GuiMenuList.h"
#include "Map_Menu_ComponentParty.h"

class Map_Menu_Inventory : public GuiMenuList, public Map_Menu_ComponentParty
{
public:
	Map_Menu_Inventory() = default;
	explicit Map_Menu_Inventory(pugi::xml_node const& node);
	~Map_Menu_Inventory() override = default;

	bool Update() override;

private:
	void HandleLeftButtonClick(int result) override;
	void HandleRightButtonClick() override;
	void InitializeElements() override;

	bool WaitingForTarget = false;

	int usedItemID = -1;
};

#endif //__MAP_MENU_INVENTORY_H__

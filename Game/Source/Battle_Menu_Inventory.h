#ifndef __BATTLE_MENU_INVENTORY_H__
#define __BATTLE_MENU_INVENTORY_H__

#include "GuiMenuList.h"
#include "Map_Menu_ComponentParty.h"

class Battle_Menu_Inventory : public GuiMenuList, public Map_Menu_ComponentParty
{
public:
	Battle_Menu_Inventory() = default;
	explicit Battle_Menu_Inventory(pugi::xml_node const& node);
	~Battle_Menu_Inventory() override = default;

	int GetUsedItemID() const;

protected:
	void SetDefaultBooleanValues() override;

private:
	void HandleLeftButtonClick(int result) override;
	void HandleRightButtonClick() override;
	void InitializeElements() override;

	int usedItemID = -1;
};

#endif //__BATTLE_MENU_INVENTORY_H__

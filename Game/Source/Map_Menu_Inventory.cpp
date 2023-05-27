#include "Map_Menu_Inventory.h"

#include "Render.h"

#include "Log.h"

Map_Menu_Inventory::Map_Menu_Inventory(pugi::xml_node const& node)
	: GuiMenuList(node)
{}

void Map_Menu_Inventory::HandleLeftButtonClick(int result)
{
	// TODO: Implement item usage here
	SetClickHandled(true);
}

void Map_Menu_Inventory::HandleRightButtonClick()
{
	ResetCurrentItemSelected();
	SetDeleteMenu(true);
	SetClickHandled(true);
}

void Map_Menu_Inventory::InitializeElements()
{
	ClearMenuItems();

	for (int i = 0; auto const& [itemID, amount] : playerParty->inventory)
	{
		std::string itemName = playerParty->dbItems->GetItem(itemID).general.name;
		std::string amountToDisplay = std::format("x{}", amount);
		int itemTextureID = playerParty->dbItems->GetItem(itemID).textureID;

		CreateMenuItem(MenuItem(MenuItem::ItemText(itemName, "", amountToDisplay), i, itemTextureID));

		i++;
	}
}

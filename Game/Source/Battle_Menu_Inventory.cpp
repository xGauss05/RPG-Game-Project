#include "Battle_Menu_Inventory.h"

#include "Render.h"

#include "Log.h"

Battle_Menu_Inventory::Battle_Menu_Inventory(pugi::xml_node const& node)
	: GuiMenuList(node)
{}

int Battle_Menu_Inventory::GetUsedItemID() const
{
	return usedItemID;
}

void Battle_Menu_Inventory::SetDefaultBooleanValues()
{
	usedItemID = -1;
	InitializeElements();
	GuiMenuList::SetDefaultBooleanValues();
}

void Battle_Menu_Inventory::HandleLeftButtonClick(int result)
{
	if (result < 0 || result >= playerParty->inventory.size())
	{
		LOG("ITEM USED %i OUT OF RANGE", result);
		return;
	}

	usedItemID = playerParty->inventory[result].first;
	SetClickHandled(true);
}


void Battle_Menu_Inventory::HandleRightButtonClick()
{
	ResetCurrentItemSelected();
	SetDeleteMenu(true);
	SetClickHandled(true);
}

void Battle_Menu_Inventory::InitializeElements()
{
	ClearMenuItems();

	for (auto const& [itemID, amount] : playerParty->inventory)
	{
		Item const& item = playerParty->dbItems->GetItem(itemID);

		if (item.general.ocasion == Item::GeneralProperties::Ocasion::NEVER
		 || item.general.ocasion == Item::GeneralProperties::Ocasion::MENU_SCREEN)
		{
			continue;
		}

		std::string itemName = item.general.name;
		std::string amountToDisplay = std::format("x{}", amount);
		int itemTextureID = item.textureID;

		CreateMenuItem(itemName, "", amountToDisplay, itemTextureID);
	}
}

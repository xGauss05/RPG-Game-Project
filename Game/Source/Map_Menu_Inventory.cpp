#include "Map_Menu_Inventory.h"

#include "Render.h"

#include "Log.h"

Map_Menu_Inventory::Map_Menu_Inventory(pugi::xml_node const& node)
	: GuiMenuList(node)
{}

bool Map_Menu_Inventory::Update()
{
	if (WaitingForTarget)
	{
		SetDefaultBooleanValues();
		WaitingForTarget = false;

		int target = GetPreviousPanelLastClick();

		if (target >= 0 && target < playerParty->party.size() && usedItemID != -1 &&
			playerParty->party[target].UseItem(playerParty->dbItems->GetItem(usedItemID)))
		{
			playerParty->RemoveItemFromInventory(usedItemID);
			InitializeElements();
		}

		usedItemID = -1;

		return false;
	}
	
	return GuiMenuList::Update();
}

void Map_Menu_Inventory::HandleLeftButtonClick(int result)
{
	if (result < 0 || result >= playerParty->inventory.size())
	{
		LOG("ITEM USED %i OUT OF RANGE", result);
		return;
	}

	usedItemID = playerParty->inventory[result].first;
	const Item &item = playerParty->dbItems->GetItem(usedItemID);

	using enum Item::GeneralProperties::Ocasion;
	using enum Item::GeneralProperties::Scope;

	if (item.general.ocasion == MENU_SCREEN || item.general.ocasion == ALWAYS)
	{
		if (item.general.scope == ALL_ALLIES || item.general.scope == ALL_DEAD_ALLIES)
		{
			bool success = false;
			for (auto& elem : playerParty->party)
			{
				if (!success)
					success = elem.UseItem(item);
				else
					elem.UseItem(item);
			}
			if (success)
				playerParty->RemoveItemFromInventory(usedItemID, 1);
		}
		else if (item.general.scope == ONE_ALLY || item.general.scope == ONE_DEAD_ALLY)
		{
			WaitingForTarget = true;
			SetClickHandled(true);
		}
	}
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

		CreateMenuItem(MenuItem(MenuItem::ItemText(itemName, "", amountToDisplay), itemTextureID));

		i++;
	}
}

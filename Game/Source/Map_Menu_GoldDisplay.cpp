#include "Map_Menu_GoldDisplay.h"

#include "Render.h"

#include "Log.h"

Map_Menu_GoldDisplay::Map_Menu_GoldDisplay(pugi::xml_node const& node)
	: GuiMenuList(node)
{}

void Map_Menu_GoldDisplay::HandleLeftButtonClick(int result)
{
}

void Map_Menu_GoldDisplay::HandleRightButtonClick()
{
}

void Map_Menu_GoldDisplay::InitializeElements()
{
	ClearMenuItems();

	int currentGold = playerParty->GetGold();

	std::string gold = "";

	if (currentGold != 0)
	{
		do
		{
			gold = std::format("{}{}", gold, currentGold % 1000);
			currentGold /= 1000;
		} while (currentGold > 0);
	}
	else
	{
		gold = "0";
	}

	int iconID = playerParty->dbItems->GetItem(0).textureID;

	CreateMenuItem(MenuItem(MenuItem::ItemText("", "", gold), 0, iconID));
}

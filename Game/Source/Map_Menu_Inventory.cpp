#include "Map_Menu_Inventory.h"
#include "Log.h"


Map_Menu_Inventory::Map_Menu_Inventory(pugi::xml_node const& node)
	: GuiMenuList(node)
{}

void Map_Menu_Inventory::HandleLeftButtonClick(int result)
{
	SetClickHandled(true);
}

void Map_Menu_Inventory::HandleRightButtonClick()
{
	SetDeleteMenu(true);
}

void Map_Menu_Inventory::InitializeElements()
{
	std::array elementNames
	{
		"Item",
		"Skill",
		"Equip",
		"Status",
		"Formation",
		"Options",
		"Save",
		"Quit Game"
	};

	std::ranges::for_each(elementNames, [this, index = 0](const char* elemName) mutable
		{
			CreateMenuItem(MenuItem(MenuItem::ItemText(elemName, "", ""), index));
			++index;
		}
	);
}

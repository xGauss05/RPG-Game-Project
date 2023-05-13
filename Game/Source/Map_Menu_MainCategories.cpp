#include "Map_Menu_MainCategories.h"
#include "Log.h"


Map_Menu_MainCategories::Map_Menu_MainCategories(pugi::xml_node const& node)
	: GuiMenuList(node)
{}

void Map_Menu_MainCategories::HandleLeftButtonClick(int result)
{
	SetClickHandled(true);
}

void Map_Menu_MainCategories::HandleRightButtonClick()
{
	SetDeleteMenu(true);
}

void Map_Menu_MainCategories::InitializeElements()
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

	for (auto const& elem : elementNames)
	{
		CreateMenuItem(MenuItem(MenuItem::ItemText(elem, "", "")));
	}
}

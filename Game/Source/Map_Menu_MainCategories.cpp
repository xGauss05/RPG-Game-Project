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
	ResetCurrentItemSelected();
	SetDeleteMenu(true);
	SetClickHandled(true);
}

void Map_Menu_MainCategories::InitializeElements()
{
	ClearMenuItems();
	
	std::array elementNames
	{
		"Items",
		"Stats"
	};

	std::ranges::for_each(elementNames, [this, index = 0](const char* elemName) mutable
		{
			CreateMenuItem(MenuItem(MenuItem::ItemText(elemName, "", ""), index));
			++index;
		}
	);
}

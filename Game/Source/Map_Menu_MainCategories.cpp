#include "Map_Menu_MainCategories.h"
#include "Log.h"
#include <array>

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
	
	std::array<std::string_view, 2> elementNames = {"Items","Stats"};

	std::ranges::for_each(elementNames, [this, index = 0](std::string_view elemName) mutable
		{
			CreateMenuItem(elemName, "", "");
			++index;
		}
	);
}

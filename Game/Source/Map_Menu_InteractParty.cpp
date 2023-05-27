#include "Map_Menu_InteractParty.h"

#include "Render.h"

#include "Log.h"

Map_Menu_InteractParty::Map_Menu_InteractParty(pugi::xml_node const& node)
	: GuiMenuList(node)
{
	SetActive(false);
}

void Map_Menu_InteractParty::HandleLeftButtonClick(int result)
{
	SetActive(false);
	SetDeleteMenu(true);
	SetClickHandled(true);
}

void Map_Menu_InteractParty::HandleRightButtonClick()
{
	SetActive(false);
	ResetCurrentItemSelected();
	SetDeleteMenu(true);
	SetClickHandled(true);
}

void Map_Menu_InteractParty::InitializeElements()
{
	ClearMenuItems();

	for (auto const& elem : playerParty->party)
	{
		CreateMenuCharacter(elem);
	}
}


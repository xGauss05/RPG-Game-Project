#include "App.h"
#include "Battle_Menu_Main.h"
#include "Log.h"
#include <array>

Battle_Menu_Main::Battle_Menu_Main(pugi::xml_node const& node)
	: GuiMenuList(node)
{
	textureID = app->tex->Load(node.child("texture").attribute("path").as_string());
}

Battle_Menu_Main::~Battle_Menu_Main()
{
	app->tex->Unload(textureID);
}

void Battle_Menu_Main::HandleLeftButtonClick(int result)
{
	SetClickHandled(true);
}

void Battle_Menu_Main::HandleRightButtonClick()
{
	SetDeleteMenu(true);
	SetClickHandled(true);
}

void Battle_Menu_Main::InitializeElements()
{
	ClearMenuItems();

	int numberOfImages = app->tex->GetSize(textureID).x / 128;

	for (int i = 0; i < numberOfImages; i += 1)
	{
		SDL_Rect rect{ i * 128, 0, 128, 128 };
		CreateMenuImage(textureID, rect);
	}

}

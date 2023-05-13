#include "Map_Window_Menu.h"
#include "Log.h"

Map_Window_Menu::Map_Window_Menu(Window_Factory const &windowFac)
{
	panels.emplace_back(windowFac.CreateMenuList("Menu_MainCategories"));
}

void Map_Window_Menu::Start()
{
	for (auto const &elem : panels)
	{
		elem->Initialize();
	}

	enabled = true;
}

void Map_Window_Menu::Update()
{
	if(!panels.empty())
		panels[currentActivePanel]->Update();
}

void Map_Window_Menu::Draw() const
{
	for (auto const& elem : panels)
	{
		elem->Draw();
	}
}

#ifndef __MAP_WINDOW_MENU_H__
#define __MAP_WINDOW_MENU_H__

#include "Window_Factory.h"

#include "Map_Menu_MainCategories.h"

#include "AdjacencyGraph.h"

class Map_Window_Menu
{
public:
	Map_Window_Menu() = default;
	explicit Map_Window_Menu(Window_Factory const &windowFac);

	void Start();

	void Update();
	void Draw() const;

private:
	std::vector<std::unique_ptr<GuiMenuList>> panels;

	bool enabled = false;



	int currentActivePanel = 0;
};

#endif //__MAP_WINDOW_MENU_H__

#ifndef __MAP_WINDOW_MENU_H__
#define __MAP_WINDOW_MENU_H__

#include "Window_Factory.h"

#include "Map_Menu_MainCategories.h"

#include "AdjacencyGraph.h"

#include "GameParty.h"

#include <stack>

class Map_Window_Menu
{
private:
	enum class MenuWindows
	{
		MAIN,
		INVENTORY,
		STATS,
		COINS,
		CHOOSE_CHARACTER
	};

public:
	Map_Window_Menu() = default;
	explicit Map_Window_Menu(Window_Factory const &windowFac);

	void InitializeLogicGraph();

	void Start();

	bool Update();
	void Draw() const;

	void SetPlayerParty(GameParty* party);

private:
	void DrawStatsMenu() const;
	void DrawPlayerStats(Battler const& character, int i) const;
	void DrawSingleStat(Battler const& character, BaseStats stat, int x, int y) const;
	
	void GoToNextPanel();
	void GoToPreviousPanel();

	std::vector<std::unique_ptr<GuiMenuList>> panels;
	std::unique_ptr<Window_Base> statsWindow;

	bool enabled = false;

	DirectedGraph<MenuWindows> menuLogic;

	int currentActivePanel = 0;
	std::stack<int> panelHistory;

	GameParty* playerParty = nullptr;

	bool bInStatsMenu = false;
};

#endif //__MAP_WINDOW_MENU_H__

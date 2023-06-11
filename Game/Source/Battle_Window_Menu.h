#ifndef __BATTLE_WINDOW_MENU_H__
#define __BATTLE_WINDOW_MENU_H__

#include "Window_Factory.h"

#include "AdjacencyGraph.h"

#include <stack>

enum class ActionNames
{
	NONE = -1,
	ATTACK = 0,
	SPECIAL_ATTACK = 1,
	DEFEND = 2,
	RUN = 3,
	ITEM = 4
};

struct BattleAction
{
	void ResetAction();

	using ItemScope = Item::GeneralProperties::Scope;

	ActionNames action = ActionNames::NONE;
	int source = -1;
	int target = -1;
	bool friendlySource = false;
	bool friendlyTarget = false;
	int speed = -1;
	int actionID = -1;
	ItemScope actionScope = ItemScope::NONE;
};

class Battle_Window_Menu
{
public:
	enum class MenuWindows
	{
		MAIN,
		FIGHT,
		FLEE,
		STATS,
		ATTACK,
		SPECIAL,
		INVENTORY,
		DEFEND,
		BATTLE_CURSOR,
		SELECT_TARGET
	};

	Battle_Window_Menu() = default;
	explicit Battle_Window_Menu(Window_Factory const &windowFac);

	void Start();

	std::pair<bool, BattleAction> Update();
	void Draw() const;

	void SetPlayerParty(GameParty* party);
	void SetEnemyTroop(EnemyTroops* enemytroop);

	bool GetCompletedStatus() const;
	bool GetInStatsMenu() const;

	void StartNewTurn();

	std::deque<BattleAction> &&GetActionQueue();

private:
	void InitializeLogicGraph();

	void DrawStatsMenu() const;
	void DrawPlayerStats(Battler const& character, int i) const;
	void DrawSingleStat(Battler const& character, BaseStats stat, int x, int y) const;
	
	void GoToNextPanel();
	void GoToPreviousPanel();
	int GetNextBattler() const;

	struct BattleCursor
	{
		int textureID = -1;
		SDL_Rect srcRect = { 0, 0, 0, 0 };
		bool enabled = false;

		int battlerTexture = -1;
		SDL_Rect actionSection = { 0, 0, 0, 0 };
		GuiSegmentedPanel actionMessage;
		int actionIconTexture = -1;
		std::string actionName = "";

		std::chrono::time_point<std::chrono::steady_clock> animTimer;
	};

	BattleAction currentAction;

	int mainMenuTexture = -1;

	std::vector<std::unique_ptr<GuiMenuList>> panels;
	std::unique_ptr<Window_Base> statsWindow;
	
	BattleCursor cursor;
	bool enabled = false;

	DirectedGraph<MenuWindows> menuLogic;

	int currentActivePanel = 0;
	std::stack<int> panelHistory;

	GameParty* playerParty = nullptr;
	int currentSource = 0;
	EnemyTroops* enemies = nullptr;
	std::vector<Battler>* currentTargetParty = nullptr;

	std::deque<BattleAction> actionQueue;

	bool bInputCompleted = false;
	bool bInStatsMenu = false;
};

#endif //__BATTLE_WINDOW_MENU_H__

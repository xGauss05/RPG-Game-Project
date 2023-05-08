#ifndef __WINDOW_LIST_H__
#define __WINDOW_LIST_H__

#include "Window_Base.h"
#include "Log.h"


class Window_List : public Window_Base
{
public:
	explicit Window_List(pugi::xml_node const& node);
	void HandleInput();
	int Update() override;

	void ResetHoveredButton();

private:
	int NewGame();
	int ContinueGame();
	int OptionsMenu();
	int ExitGameFromTitle();
	int PauseGame();
	int ResumeGame();
	int ExitMainMenu();
	int ExitGameFromMap();
	int ExitOptions();
	

	int BattleAttack();
	int BattleSpecialAttack();
	int BattleDefend();
	int BattleRun();

	int DialogYes();
	int DialogNo();

	int currentHoveredButton = 0;
};

#endif __WINDOW_LIST_H__
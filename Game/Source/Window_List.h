#ifndef __WINDOW_LIST_H__
#define __WINDOW_LIST_H__

#include "Window_Base.h"
#include "Log.h"


class Window_List : public Window_Base
{
public:
	explicit Window_List(pugi::xml_node const& node);

private:
	int NewGame();
	int ContinueGame();
	int OptionsWindow();
	int ExitGameFromTitle();
	int PauseGame();
	int ResumeGame();
	int ExitMainMenu();
	int ExitGameFromMap();
	

	int BattleAttack();
	int BattleSpecialAttack();
	int BattleDefend();
	int BattleRun();

	int DialogYes();
	int DialogNo();
};

#endif __WINDOW_LIST_H__
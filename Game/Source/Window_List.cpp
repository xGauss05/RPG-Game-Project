#include "Window_List.h"
#include "SceneManager.h"
#include "Scene_Title.h"
#include "Scene_Map.h"
#include "Log.h"

Window_List::Window_List(pugi::xml_node const& node) : Window_Base(node)
{
	AddFunctionToMap("NewGame", std::bind_front(&Window_List::NewGame, this));
	AddFunctionToMap("ContinueGame", std::bind_front(&Window_List::ContinueGame, this));
	AddFunctionToMap("OptionsWindow", std::bind_front(&Window_List::OptionsWindow, this));
	AddFunctionToMap("ExitGameFromTitle", std::bind_front(&Window_List::ExitGameFromTitle, this));
	AddFunctionToMap("PauseGame", std::bind_front(&Window_List::PauseGame, this));
	AddFunctionToMap("ResumeGame", std::bind_front(&Window_List::ResumeGame, this));
	AddFunctionToMap("ExitMainMenu", std::bind_front(&Window_List::ExitMainMenu, this));
	AddFunctionToMap("ExitGameFromMap", std::bind_front(&Window_List::ExitGameFromMap, this));

	AddFunctionToMap("BattleAttack", std::bind_front(&Window_List::BattleAttack, this));
	AddFunctionToMap("BattleSpecialAttack", std::bind_front(&Window_List::BattleSpecialAttack, this));
	AddFunctionToMap("BattleDefend", std::bind_front(&Window_List::BattleDefend, this));
	AddFunctionToMap("BattleRun", std::bind_front(&Window_List::BattleRun, this));

	AddFunctionToMap("DialogYes", std::bind_front(&Window_List::DialogYes, this));
	AddFunctionToMap("DialogNo", std::bind_front(&Window_List::DialogNo, this));
	
	CreateButtons(node);
}

int Window_List::NewGame()
{
	LOG("NewGame function called");
	return 1;
}

int Window_List::ContinueGame()
{
	LOG("ContinueGame function called");
	return 2;
}

int Window_List::OptionsWindow()
{
	LOG("OptionsWindow function called");
	return 3;
}

int Window_List::ExitGameFromTitle() 
{
	LOG("ExitGameFromTitle function called");
	
	return 4;
}

int Window_List::PauseGame() {
	LOG("PauseGame function called");
	app->pause = true;
	if(!app->PauseGame()) return 4;
		
	return 0;
}

int Window_List::ResumeGame() 
{
	LOG("ResumeGame function called");
	app->pause = false;
	app->PauseGame();

	return 6;
}

int Window_List::ExitMainMenu() 
{
	LOG("ExitMainMenu function called");
	
	app->pause = false;
	app->PauseGame();

	return 7;
}

int Window_List::ExitGameFromMap()
{
	LOG("ExitGameFromMap function called");
	app->pause = false;
	app->PauseGame();

	return 4;
}

int Window_List::BattleAttack()
{
	return 100;
}

int Window_List::BattleSpecialAttack()
{
	return 101;
}

int Window_List::BattleDefend()
{
	return 102;
}

int Window_List::BattleRun()
{
	return 103;
}

int Window_List::DialogYes()
{
	return 200;
}

int Window_List::DialogNo()
{
	return 201;
}

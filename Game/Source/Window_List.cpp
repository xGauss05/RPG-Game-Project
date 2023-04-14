#include "Window_List.h"

#include "Log.h"

Window_List::Window_List(pugi::xml_node const& node) : Window_Base(node)
{
	AddFunctionToMap("NewGame", std::bind_front(&Window_List::NewGame, this));
	AddFunctionToMap("ContinueGame", std::bind_front(&Window_List::ContinueGame, this));
	AddFunctionToMap("OptionsWindow", std::bind_front(&Window_List::OptionsWindow, this));
	AddFunctionToMap("ExitGame", std::bind_front(&Window_List::ExitGame, this));
	AddFunctionToMap("PauseGame", std::bind_front(&Window_List::PauseGame, this));
	AddFunctionToMap("ResumeGame", std::bind_front(&Window_List::ResumeGame, this));
	AddFunctionToMap("ExitMainMenu", std::bind_front(&Window_List::ExitMainMenu, this));
	
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

int Window_List::ExitGame() 
{
	LOG("ExitGame function called");
	return 4;
}

int Window_List::PauseGame() {
	LOG("PauseGame function called");
	app->pause = true;
	app->PauseGame();
		
	return 5;
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
	app->PauseGame();
	
	return 7;
}

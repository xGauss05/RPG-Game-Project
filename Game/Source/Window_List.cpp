#include "Window_List.h"

#include "Log.h"

Window_List::Window_List(pugi::xml_node const& node) : Window_Base(node)
{
	AddFunctionToMap("NewGame", std::bind_front(&Window_List::NewGame, this));
	AddFunctionToMap("ContinueGame", std::bind_front(&Window_List::ContinueGame, this));
	AddFunctionToMap("OptionsWindow", std::bind_front(&Window_List::OptionsWindow, this));
	AddFunctionToMap("ExitGame", std::bind_front(&Window_List::ExitGame, this));
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
	return 0;
}

int Window_List::OptionsWindow()
{
	LOG("OptionsWindow function called");
	return 0;
}

int Window_List::ExitGame() 
{
	LOG("ExitGame function called");
	return 0;
}

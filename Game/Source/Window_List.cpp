#include "Window_List.h"

#include "Log.h"

Window_List::Window_List(pugi::xml_node const& node) : Window_Base(node)
{
	AddFunctionToMap("NewGame", std::bind_front(&Window_List::NewGame, this));
	CreateButtons(node);
}

int Window_List::NewGame()
{
	LOG("NewGame function called");
	return 1;
}

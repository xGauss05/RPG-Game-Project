#include "Window_Panel.h"

#include "Log.h"

Window_Panel::Window_Panel(pugi::xml_node const& node) : Window_Base(node)
{
	CreatePanels(node);
}

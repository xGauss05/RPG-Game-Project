#include "Window_Panel.h"
#include "GuiBox.h"
#include "Log.h"

Window_Panel::Window_Panel(pugi::xml_node const& node) : Window_Base(node)
{
	CreatePanels(node);
}

void Window_Panel::ModifyLastWidgetText(std::string_view s)
{
	auto* currentWidget = dynamic_cast<GuiBox*>(AccessLastWidget());
	currentWidget->ModifyText(s);
}

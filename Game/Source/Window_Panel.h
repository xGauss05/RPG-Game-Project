#ifndef __WINDOW_PANEL_H__
#define __WINDOW_PANEL_H__

#include "Window_Base.h"
#include "Log.h"


class Window_Panel : public Window_Base
{
public:
	explicit Window_Panel(pugi::xml_node const& node);
	std::string_view ReadLastWidgetText();
	void ModifyLastWidgetText(std::string_view s);

private:

};

#endif __WINDOW_PANEL_H__
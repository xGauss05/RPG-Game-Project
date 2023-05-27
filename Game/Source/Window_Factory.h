#ifndef __Window_Factory_H__
#define __Window_Factory_H__

#include "Window_Base.h"
#include "Window_List.h"
#include "Window_Panel.h"
#include "GuiMenuList.h"
#include "Map_Menu_MainCategories.h"
#include "Map_Menu_Inventory.h"
#include "Map_Menu_InteractParty.h"
#include "Map_Display_QuestLog.h"
#include "Map_Menu_GoldDisplay.h"

#include "PugiXml/src/pugixml.hpp"

class Window_Factory
{
public:
	explicit Window_Factory(pugi::xml_node const& windowInfo);

	std::unique_ptr<Window_Base> CreateWindow(std::string_view const& windowName) const;
	std::unique_ptr<Window_List> CreateWindowList(std::string_view const& windowName = "ListFallback") const;
	std::unique_ptr<Window_Panel> CreateWindowPanel(std::string_view const& windowName = "PanelFallback") const;
	std::unique_ptr<GuiMenuList> CreateMenuList(std::string_view const& windowName = "MenuListFallback") const;

	std::unique_ptr<Map_Display_QuestLog> CreateQuestLog() const;

	std::unique_ptr<Map_Menu_GoldDisplay> CreateGoldDisplay() const;

private:
	LookUpXMLNodeFromString info;
	int fCleanCraters;
};

#endif __Window_Factory_H__
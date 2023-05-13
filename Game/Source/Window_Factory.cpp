#include "Window_Factory.h"
#include "TextManager.h"

#include "Log.h"

Window_Factory::Window_Factory(pugi::xml_node const& windowInfo)
{
	for (auto const& node : windowInfo.child("window_info").children("window"))
	{
		info[node.attribute("name").as_string()] = node;
	}
}

std::unique_ptr<Window_Base> Window_Factory::CreateWindow(std::string_view const& windowName) const
{
	auto windowHash = info.find(windowName);
	if (windowHash == info.end())
	{
		LOG("Window information for %s not found in XML.", windowName);
		return nullptr;
	}
	
	std::unique_ptr<Window_Base> returnPtr = nullptr;

	if (StrEquals("List", windowHash->second.attribute("class").as_string()))
	{
		if (StrEquals("Base", windowHash->second.attribute("type").as_string()))
		{
			returnPtr = std::make_unique<Window_List>(windowHash->second);
		}
	}
	else if (StrEquals("Panel", windowHash->second.attribute("class").as_string()))
	{
		if (StrEquals("Base", windowHash->second.attribute("type").as_string()))
		{
			returnPtr = std::make_unique<Window_Panel>(windowHash->second);
		}
		else if (StrEquals("Message", windowHash->second.attribute("type").as_string()))
		{
			returnPtr = std::make_unique<Window_Panel>(windowHash->second);
		}
	} 

	return returnPtr;
}

std::unique_ptr<Window_List> Window_Factory::CreateWindowList(std::string_view const& windowName) const
{
	std::unique_ptr<Window_List> returnPtr = nullptr;

	auto windowHash = info.find(windowName);
	if (windowHash == info.end())
	{
		LOG("Window information for %s not found in XML.", windowName);
		returnPtr = std::make_unique<Window_List>(info.find("ListFallback")->second);
	}
	else
		returnPtr = std::make_unique<Window_List>(windowHash->second);

	return returnPtr;
}

std::unique_ptr<Window_Panel> Window_Factory::CreateWindowPanel(std::string_view const& windowName) const
{
	std::unique_ptr<Window_Panel> returnPtr = nullptr;

	auto windowHash = info.find(windowName);
	if (windowHash == info.end())
	{
		LOG("Window information for %s not found in XML.", windowName);
		returnPtr = std::make_unique<Window_Panel>(info.find("PanelFallback")->second);
	}
	else
		returnPtr = std::make_unique<Window_Panel>(windowHash->second);

	return returnPtr;
}

std::unique_ptr<GuiMenuList> Window_Factory::CreateMenuList(std::string_view const& windowName) const
{
	std::unique_ptr<GuiMenuList> returnPtr = nullptr;

	auto windowHash = info.find(windowName);
	if (windowHash == info.end())
	{
		LOG("Window information for %s not found in XML.", windowName);
		//returnPtr = std::make_unique<GuiMenuList>(info.find("MenuListFallback")->second);
	}
	else
		//returnPtr = std::make_unique<GuiMenuList>(windowHash->second);

	return returnPtr;
}

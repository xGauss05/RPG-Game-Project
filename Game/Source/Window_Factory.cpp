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

	return returnPtr;
}

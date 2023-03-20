#include "Event_Teleport.h"

#include <format>
#include <string>

void Event_Teleport::parseXMLProperties(pugi::xml_node const& node)
{
	for (auto const& child : node.children())
	{
		auto attributeName = child.attribute("name").as_string();
		
		if (StrEquals("Base", attributeName))
		{
			common.ReadProperty(child);
		}
		else if (StrEquals("Destination", attributeName))
		{
			destination.ReadProperty(child);
		}
		// GlobalSwitch1, GlobalSwitch2... Way to add multiple global switches to a single chest.
		else if (StrEquals(std::format("{}{}", "GlobalSwitch", std::to_string(globalSwitch.size() + 1)), attributeName))
		{
			globalSwitch.emplace_back(std::make_unique<EventProperties::GlobalSwitchProperty>());
			globalSwitch.back()->ReadProperty(child);
		}
	}
}

void Event_Teleport::Create(pugi::xml_node const &node)
{
	Event_Base::Initialize(node);
}

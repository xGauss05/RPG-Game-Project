#include "Event_Lever.h"

#include "Log.h"

#include <format>
#include <string>

void Event_Lever::parseXMLProperties(pugi::xml_node const& node)
{
	for (auto const& child : node.children())
	{
		auto attributeName = child.attribute("name").as_string();
		
		if (StrEquals("Base", attributeName))
		{
			common.ReadProperty(child);
		}
		// GlobalSwitch1, GlobalSwitch2... Way to add multiple global switches to a single lever.
		else if (StrEquals(std::format("{}{}", "GlobalSwitch", std::to_string(globalSwitch.size() + 1)), attributeName))
		{
			globalSwitch.emplace_back();
			globalSwitch.back().ReadProperty(child);
		}
		else
		{
			LOG("Lever property %s not implemented.", attributeName);
		}
	}
}

EventTrigger Event_Lever::OnTrigger()
{
	state = !state;

	EventTrigger returnTrigger;

	returnTrigger.eventFunction = EventTrigger::WhatToDo::GLOBAL_SWITCH;

	returnTrigger.values.emplace_back(type, id);

	returnTrigger.globalSwitchIteratorBegin = globalSwitch.begin();
	returnTrigger.globalSwitchIteratorEnd = globalSwitch.end();

	return returnTrigger;
}

void Event_Lever::Create(pugi::xml_node const& node)
{
	Sprite::Initialize(node);
	Event_Base::Initialize(node);
	SetInteractedGid();
}
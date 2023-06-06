#include "Event_Boss_Combat.h"

#include "Log.h"

#include <format>
#include <string>

void Event_Boss_Combat::parseXMLProperties(pugi::xml_node const& node)
{
	NPC_Generic::parseXMLProperties(node);

	for (auto const& child : node.children())
	{
		auto attributeType = child.attribute("propertytype").as_string();
		
		if (StrEquals("Global Switch", attributeType))
		{
			globalSwitch.emplace_back();
			globalSwitch.back().ReadProperty(child);
		}
	}
}

EventTrigger Event_Boss_Combat::OnTrigger()
{
	EventTrigger returnTrigger;
	returnTrigger.text = GetDialoguePath();
	returnTrigger.eventFunction = EventTrigger::WhatToDo::DIALOG_PATH;

	return returnTrigger;
}

void Event_Boss_Combat::Create(pugi::xml_node const& node)
{
	Event_Base::Initialize(node);
}
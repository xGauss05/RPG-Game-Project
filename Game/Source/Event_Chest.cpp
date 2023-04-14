#include "Event_Chest.h"

#include "Log.h"

#include <format>
#include <string>

void Event_Chest::parseXMLProperties(pugi::xml_node const& node)
{
	for (auto const& child : node.children())
	{
		auto attributeName = child.attribute("name").as_string();

		if (StrEquals("isLocked", attributeName))
		{
			isLocked = child.attribute("value").as_bool();
		}
		else if (StrEquals("isOpen", attributeName))
		{
			isOpen = child.attribute("value").as_bool();
		}
		else if (StrEquals("Base", attributeName))
		{
			common.ReadProperty(child);
		}
		// Loot1, Loot2, Loot3... Way to add multiple loots to a single chest.
		else if (StrEquals(std::format("{}{}", "Loot", std::to_string(loot.size() + 1)), attributeName))
		{
			loot.emplace_back(std::make_unique<EventProperties::LootProperty>());
			loot.back()->ReadProperty(child);
		}
		// GlobalSwitch1, GlobalSwitch2... Way to add multiple global switches to a single chest.
		else if (StrEquals(std::format("{}{}", "GlobalSwitch", std::to_string(globalSwitch.size() + 1)), attributeName))
		{
			globalSwitch.emplace_back(std::make_unique<EventProperties::GlobalSwitchProperty>());
			globalSwitch.back()->ReadProperty(child);
		}
		else
		{
			LOG("Chest property %s not implemented.", attributeName);
		}
	}
}

EventTrigger Event_Chest::OnTrigger()
{
	EventTrigger returnTrigger;

	if (isOpen)
	{
		returnTrigger.eventFunction = EventTrigger::WhatToDo::NO_EVENT;
		return returnTrigger;
	}

	if (isLocked)
	{
		returnTrigger.text = "Chest is locked.";
		returnTrigger.eventFunction = EventTrigger::WhatToDo::SHOW_MESSAGE;
		return returnTrigger;
	}
	
	isOpen = true;
	returnTrigger.text = "You find {} {}.";
	returnTrigger.eventFunction = EventTrigger::WhatToDo::LOOT;
	for (auto const& elem : loot)
	{
		returnTrigger.values.emplace_back(elem->item, elem->quantity);
	}

	return returnTrigger;
}

void Event_Chest::Create(pugi::xml_node const& node)
{
	Sprite::Initialize(node);
	Event_Base::Initialize(node);
}
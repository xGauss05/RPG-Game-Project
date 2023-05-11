#include "NPC_Generic.h"

#include "Log.h"

#include <format>
#include <string>

void NPC_Generic::parseXMLProperties(pugi::xml_node const& node)
{
	bool numberOfTilesChecked = false;
	for (auto const& child : node.children())
	{
		auto attributeName = child.attribute("name").as_string();

		if (StrEquals("Static", attributeName))
		{
			isStatic = child.attribute("value").as_bool();
		}
		else if (StrEquals("Dialogue", attributeName))
		{
			hasDialogue = child.attribute("value").as_bool();
		}
		else if (StrEquals("DialoguePath", attributeName))
		{
			dialoguePath = child.attribute("value").as_string();
			if(!dialoguePath.empty() && dialoguePath.size() >= 6) 
			{
				dialoguePath.erase(0, 6);
			}
		}
		else if (StrEquals("Base", attributeName))
		{
			common.ReadProperty(child);
		}
		else if (StrEquals("isOneTile", attributeName))
		{
			bIsTwoTiles = !child.attribute("value").as_bool();
			numberOfTilesChecked = true;
		}
		else
		{
			LOG("NPC_Base property %s not implemented.", attributeName);
		}
	}

	if (!numberOfTilesChecked)
		bIsTwoTiles = true;
}

EventTrigger NPC_Generic::OnTrigger()
{
	EventTrigger returnTrigger;
	returnTrigger.text = dialoguePath;
	returnTrigger.eventFunction = EventTrigger::WhatToDo::DIALOG_PATH;

	return returnTrigger;
}

void NPC_Generic::Create(pugi::xml_node const& node)
{
	Sprite::Initialize(node);
	Event_Base::Initialize(node);
}
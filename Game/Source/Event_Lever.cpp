#include "Event_Lever.h"
#include "Audio.h"
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

	if (state) 
	{
		app->audio->PlayFx(levelOpenSfx);
		SetAnimation(AnimationState::STATE_ON);
	}
	else 
	{
		app->audio->PlayFx(leverCloseSfx);
		SetAnimation(AnimationState::STATE_OFF);
	}

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

	leverCloseSfx = app->audio->LoadFx("Fx/S_Dungeon-Lever-Close.wav");
	levelOpenSfx = app->audio->LoadFx("Fx/S_Dungeon-Lever-Open.wav");
}
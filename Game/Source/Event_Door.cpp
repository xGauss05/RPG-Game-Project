#include "Event_Door.h"

#include "Log.h"

#include <format>
#include <string>

Event_Door::Event_Door(Publisher& pub)
	: Subscriber(pub)
{}

void Event_Door::parseXMLProperties(pugi::xml_node const& node)
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
			switchesState[globalSwitch.back().id] = false;
		}
		else if (StrEquals("state", attributeName))
		{
			state = child.attribute("value").as_bool();
		}
		else
		{
			LOG("Torch property %s not implemented.", attributeName);
		}
	}

}

EventTrigger Event_Door::OnTrigger()
{
	return EventTrigger();
}

void Event_Door::Create(pugi::xml_node const& node)
{
	Event_Base::Initialize(node);
	AttachToGlobalSwitches();
	SetInteractedGid();
}

void Event_Door::UpdateSubscriber(int id, bool s)
{
	LOG(std::format("Event Door updated via switch {} id to state {}", id, state).c_str());

	switchesState[id] = s;

	for (auto const& [index, gsState] : switchesState)
	{
		if (gsState == false)
			return;
	}

	common.isActive = false;
}

void Event_Door::AttachToGlobalSwitches()
{
	for (auto const& elem : globalSwitch)
	{
		if(elem.functionOnInteract == EventProperties::GlobalSwitchOnInteract::QUERY)
			AttachToPublisherID(elem.id);
	}
}

#include "Event_Torch.h"

#include "Log.h"

#include <format>
#include <string>

Event_Torch::Event_Torch(Publisher& pub)
	: Subscriber(pub)
{}

void Event_Torch::parseXMLProperties(pugi::xml_node const& node)
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
			LOG("Torch property %s not implemented.", attributeName);
		}
	}
}

EventTrigger Event_Torch::OnTrigger()
{
	return EventTrigger();
}

void Event_Torch::Create(pugi::xml_node const& node)
{
	Sprite::Initialize(node);
	Event_Base::Initialize(node);
	AttachToGlobalSwitches();
}

void Event_Torch::UpdateSubscriber(int id, bool s)
{
	state = !state;
	LOG(std::format("Event Torch updated via switch {} id to state {}", id, state).c_str());
}

void Event_Torch::AttachToGlobalSwitches()
{
	for (auto const& elem : globalSwitch)
	{
		if(elem.functionOnInteract == EventProperties::GlobalSwitchOnInteract::QUERY)
			AttachToPublisherID(elem.id);
	}
}

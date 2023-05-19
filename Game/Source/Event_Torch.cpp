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
		if (std::string_view attributeType = child.attribute("propertytype").as_string();
			!attributeType.empty())
		{
			if (StrEquals("Base Event", attributeType))
			{
				common.ReadProperty(child);
			}
			// GlobalSwitch1, GlobalSwitch2... Way to add multiple global switches to a single lever.
			else if (StrEquals("Global Switch", attributeType))
			{
				globalSwitch.emplace_back();
				globalSwitch.back().ReadProperty(child);
			}
		}
		else
		{
			if (StrEquals("state", child.attribute("name").as_string()))
			{
				state = child.attribute("value").as_bool();
			}
			else
			{
				LOG("Torch property %s not implemented.", child.attribute("name").as_string());
			}
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
	SetInteractedGid();
}

void Event_Torch::UpdateSubscriber(int id, bool s)
{
	state = !state;

	for (auto const& elem : globalSwitch)
	{
		if (elem.functionOnInteract == EventProperties::GlobalSwitchOnInteract::SAME_VALUE_AS_STATE)
		{
			UpdateGlobalSwitchValue(elem.id, state);
		}
	}

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

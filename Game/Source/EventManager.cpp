#include "EventManager.h"
#include "App.h"

#include "Event_Chest.h"
#include "Event_Lever.h"
#include "Event_Torch.h"
#include "Event_Door.h"
#include "Event_Teleport.h"
#include "NPC_Generic.h"
#include "Event_Boss_Combat.h"

#include "Sprite.h"

#include "Log.h"

EventManager::EventManager() = default;

// Destructor
EventManager::~EventManager() = default;

void EventManager::Initialize()
{
	if (!events.empty())
		drawIterator = events.begin();
}

bool EventManager::CreateEvent(Publisher& publisher, pugi::xml_node const& node)
{
	for (auto const& child : node.children("object"))
	{
		std::unique_ptr<Event_Base> event = nullptr;

		if (StrEquals("Event Chest", child.attribute("type").as_string()))
		{
			event = std::make_unique<Event_Chest>();
		}
		else if (StrEquals("Event Teleport", child.attribute("type").as_string()))
		{
			event = std::make_unique<Event_Teleport>();
			event->walkable = true;
		}
		else if (StrEquals("Event NPC", child.attribute("type").as_string()))
		{
			event = std::make_unique<NPC_Generic>();
		}
		else if (StrEquals("Event Lever", child.attribute("type").as_string()))
		{
			event = std::make_unique<Event_Lever>();
		}
		else if (StrEquals("Event Torch", child.attribute("type").as_string()))
		{
			event = std::make_unique<Event_Torch>(publisher);
		}
		else if (StrEquals("Event Door", child.attribute("type").as_string()))
		{
			event = std::make_unique<Event_Door>(publisher);
		}
		else if (StrEquals("Event Boss Combat", child.attribute("type").as_string()))
		{
			event = std::make_unique<Event_Boss_Combat>();
		}

		if (!event)
		{
			LOG("Event \"%s\" not yet implemented.", child.attribute("type").as_string());
			continue;
		}

		event->type = child.attribute("type").as_string();
		event->Create(child);
		events.push_back(std::move(event));
	}
	return true;
}

void EventManager::SubscribeEventsToGlobalSwitches()
{
	for (auto const& elem : events)
	{
		elem->AttachToGlobalSwitches();
	}
}

int EventManager::GetEventLayerSize() const
{
	return events.size();
}

bool EventManager::IsWalkable(iPoint position) const
{
	position = position * 48;

	for (auto const& elem : events)
	{
		iPoint eventPos = elem->position;

		if (!elem->IsEventActive() || elem->walkable)
			continue;

		if (eventPos == position)
			return false;

	}

	return true;
}

EventTrigger EventManager::TriggerEvent(iPoint destination) const
{
	for (auto const& event : events)
	{
		if (!event->IsEventActive())
		{
			continue;
		}

		if (event->common.trigger == EventProperties::EventTriggerOn::ACTION_BUTTON
			&& event->position == destination)
		{
			return event->OnTrigger();
		}
	}

	return EventTrigger();
}

EventTrigger EventManager::TriggerFloorEvent(iPoint destination) const
{
	for (auto const& event : events)
	{
		// We ignore the event if it's not active nor is player touch actived
		if (!event->IsEventActive()
			|| event->common.trigger != EventProperties::EventTriggerOn::PLAYER_TOUCH)
		{
			continue;
		}

		if (event->position / 48 == destination / 48)
		{
			return event->OnTrigger();
		}
	}

	return EventTrigger();
}
 
std::tuple<int, iPoint, bool> EventManager::GetDrawEventInfo([[maybe_unused]] int index)
{
	if (events.empty() || drawIterator == events.end())
		return std::make_tuple(0, iPoint(0, 0), false);

	auto sprite = dynamic_cast<Sprite*>(drawIterator->get());
	
	if (!sprite)
	{
		++drawIterator;
		
		if (drawIterator == events.end())
		{
			drawIterator = events.begin();
			return std::make_tuple(0, iPoint(0, 0), false);
		}
		return std::make_tuple(0, iPoint(0, 0), true);
	}

	auto gid = sprite->GetGid(drawIterator->get()->state);

	auto pos = dynamic_cast<Transform*>(drawIterator->get())->GetPosition();
	
	do {
		++drawIterator;
		if (drawIterator == events.end())
		{
			drawIterator = events.begin();
			return std::make_tuple(gid, pos, false);
		}
	} while (!drawIterator->get()->IsEventActive());

	return std::make_tuple(gid, pos, true);
}



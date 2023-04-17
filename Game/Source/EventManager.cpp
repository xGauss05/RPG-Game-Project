#include "EventManager.h"
#include "App.h"

#include "Event_Chest.h"
#include "Event_Teleport.h"
#include "NPC_Generic.h"

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

bool EventManager::CreateEvent(pugi::xml_node const& node)
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
		}
		else if (StrEquals("Event NPC", child.attribute("type").as_string()))
		{
			event = std::make_unique<NPC_Generic>();
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

int EventManager::GetEventLayerSize() const
{
	return events.size();
}

EventTrigger EventManager::TriggerEvent(iPoint destination) const
{
	for (auto const& event : events)
	{
		if (event->common.trigger == EventProperties::EventTriggerOn::PLAYER_TOUCH && event->position == destination)
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
		if (event->common.trigger == EventProperties::EventTriggerOn::EVENT_TOUCH && event->position == destination)
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

	auto gid = sprite->GetGid();

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



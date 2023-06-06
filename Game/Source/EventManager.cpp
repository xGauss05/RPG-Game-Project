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

#include <cstdlib>
#include <array>

EventManager::EventManager() = default;

// Destructor
EventManager::~EventManager() = default;

void EventManager::Initialize()
{
	periodicSFXs.clear();
}

bool EventManager::CreateEvents(Publisher& publisher, pugi::xml_node const& node)
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

		event->Create(child);
		
		if (auto const& SFXNode = child.child("properties").find_child_by_attribute("name", "PeriodicSFX");
			SFXNode)
		{
			auto const &SFXProperties = SFXNode.child("properties");

			std::string_view pathNode = SFXProperties.find_child_by_attribute("name", "SFX").attribute("value").as_string();

			bool sfxAlreadyInVector = std::ranges::any_of(
				   periodicSFXs,
				   [pathNode](AmbienceSFX const& sfx)
				   {
						  return StrEquals(pathNode, sfx.path);
				   }
			);

			if (!sfxAlreadyInVector)
			{
				periodicSFXs.emplace_back(SFXNode);
			}

		}

		events.push_back(std::move(event));
	}

	LoadEventTextures();

	return true;
}

void EventManager::SubscribeEventsToGlobalSwitches() const
{
	for (auto const& elem : events)
	{
		elem->AttachToGlobalSwitches();
	}
}

bool EventManager::IsWalkable(iPoint position) const
{
	position = position * 48;

	for (auto const& elem : events)
	{
		if (!elem->IsEventActive() || elem->walkable)
			continue;

		iPoint eventPos = elem->position;

		auto sprite = dynamic_cast<Sprite*>(elem.get());

		if (sprite)
		{
			iPoint eventSize = sprite->GetSize();
			eventPos.y += eventSize.y;
			eventPos.y = eventPos.y - tileSize;

			iPoint originalPosition = eventPos;

			while (eventPos.x < originalPosition.x + eventSize.x)
			{
				if (eventPos == position)
					return false;

				eventPos.x += tileSize;
			}
		}
		else
		{
			if (eventPos == position)
				return false;
		}

	}

	return true;
}

void EventManager::DrawEvents() const
{
	for (auto const& elem : events)
	{
		if (auto sprite = dynamic_cast<Sprite*>(elem.get());
			sprite)
		{
			sprite->Draw(elem->GetPosition());
		}
	}
}

EventTrigger EventManager::TriggerActionButtonEvent(iPoint position) const
{
	for (auto const& event : events)
	{
		if (!event->IsEventActive())
		{
			continue;
		}
		
		if (event->common.trigger == EventProperties::EventTriggerOn::ACTION_BUTTON)
		{
			if (auto [returnValue, success] = TriggerEvent(position, event.get());
				success)
			{
				return returnValue;
			}
		}
	}

	return EventTrigger();
}

EventTrigger EventManager::TriggerPlayerTouchEvent(iPoint position) const
{
	for (auto const& event : events)
	{
		// We ignore the event if it's not active nor is player touch actived
		if (!event->IsEventActive())
		{
			continue;
		}

		if (event->common.trigger == EventProperties::EventTriggerOn::PLAYER_TOUCH)
		{
			if (auto [returnValue, success] = TriggerEvent(position, event.get());
				success)
			{
				return returnValue;
			}
		}
	}

	return EventTrigger();
}

std::vector<AmbienceSFX>& EventManager::GetPeriodicSFXs()
{
	return periodicSFXs;
}

void EventManager::LoadEventTextures() const
{
	pugi::xml_document doc;
	if (auto result = doc.load_file(eventsTileset.c_str());
		!result)
	{
		LOG("Could not load event tileset %s. XML error %s", eventsTileset, result.description());
		return;
	}

	pugi::xml_node tileset = doc.child("tileset");

	for (auto const& elem : events)
	{
		auto sprite = dynamic_cast<Sprite*>(elem.get());

		if (sprite)
		{
			int gid = sprite->GetGid() - firstGID;
			for (auto const& tileNode : tileset)
			{
				if (tileNode.attribute("id").as_int() == gid)
					sprite->LoadTextures(tileNode);
			}
		}
	}
}

std::pair<EventTrigger, bool> EventManager::TriggerEvent(iPoint position, Event_Base * event) const
{
	iPoint eventPos = event->GetPosition();
	iPoint eventSize = event->GetSize();

	if (PointInRect(position, eventPos, eventSize))
	{
		return { event->OnTrigger(), true };
	}

	return { EventTrigger(), false };
}

void EventManager::SetEventsTilesetPath(std::string_view path, int gid)
{
	while(path.starts_with("../"))
		path.remove_prefix(3);

	eventsTileset = std::format("Assets/{}", path);
	firstGID = gid;
}

AmbienceSFX::AmbienceSFX(pugi::xml_node const& node)
{
	ReadProperty(node);
}

void AmbienceSFX::ReadProperty(pugi::xml_node const& node)
{
	auto propertiesNode = node.child("properties");

	for (auto const& child : propertiesNode.children("property"))
	{
		auto attributeName = child.attribute("name").as_string();
		if (StrEquals("Cooldown (seconds)", attributeName))
		{
			baseCooldown = child.attribute("value").as_int();
		}
		else if (StrEquals("Cooldown variance (%)", attributeName))
		{
			cooldownVariance = child.attribute("value").as_int();
		}
		else if (StrEquals("Max Cooldown", attributeName))
		{
			maxCooldown = child.attribute("value").as_int();
		}
		else if (StrEquals("Min Cooldown", attributeName))
		{
			minCooldown = child.attribute("value").as_int();
		}
		else if (StrEquals("Random chance (%)", attributeName))
		{
			randomChance = child.attribute("value").as_int();
		}
		else if (StrEquals("SFX", attributeName))
		{
			path = child.attribute("value").as_string();
		}
	}
}

void AmbienceSFX::CalculateNewCooldown()
{
	srand(time(NULL));

	float variance = 0;

	if(cooldownVariance > 0)
		variance = static_cast<float>((rand() % (cooldownVariance * 2)) - cooldownVariance) / 100.0f;

	float varianceValue = static_cast<float>(baseCooldown) * variance;

	cooldown = baseCooldown + static_cast<int>(ceil(varianceValue));
}

#ifndef __EVENT_BASE_H__
#define __EVENT_BASE_H__


#include "Point.h"
#include "Defs.h"
#include "PugiXml/src/pugixml.hpp"
#include <SDL/include/SDL_pixels.h>

namespace EventProperties
{
	struct Property
	{
		virtual ~Property() = default;
		virtual void ReadProperty(pugi::xml_node const& node) = 0;
	};

	enum class EventTriggerOn
	{
		ACTION_BUTTON,			// When user presses interact button.
		PLAYER_TOUCH,			// When player character tries to walk into the event.
		EVENT_TOUCH,			// When event triest to walk into player.
		AUTORRUN,				// When player enters the map, freezes all other events and player inputs until end of event.
		PARALLEL				// When player enters the map, lets user keep using inputs and other events running.
	};

	struct CommonProperties : public Property
	{
		std::string sfxPath = "";
		EventTriggerOn trigger = EventTriggerOn::ACTION_BUTTON;
		bool isActive = true;

		void ReadProperty(pugi::xml_node const& node) override
		{
			auto propertiesNode = node.child("properties");

			for (auto const& child : propertiesNode.children("property"))
			{
				auto attributeName = child.attribute("name").as_string();
				if (StrEquals("SFX", attributeName))
				{
					sfxPath = child.attribute("value").as_string();
				}
				else if (StrEquals("Trigger", attributeName))
				{
					trigger = static_cast<EventTriggerOn>(child.attribute("value").as_int());
				}
				else if (StrEquals("isActive", attributeName))
				{
					isActive = child.attribute("value").as_bool();
				}
			}
		}
	};

	struct LootProperty : public Property
	{
		std::string item = "";	// TODO Change from string to Item in database (index maybe?)
		int quantity = 0;		// Can be negative if we want to take items from player.

		void ReadProperty(pugi::xml_node const& node) override
		{
			auto propertiesNode = node.child("properties");

			for (auto const& child : propertiesNode.children("property"))
			{
				auto attributeName = child.attribute("name").as_string();
				if (StrEquals("Item", attributeName))
				{
					item = child.attribute("value").as_string();
				}
				else if (StrEquals("Quantity", attributeName))
				{
					quantity = child.attribute("value").as_int();
				}
			}
		}
	};

	enum class GlobalSwitchOnInteract
	{
		NONE,					// Default value, does nothing
		SET,					// Sets the global switch on a certain value (e.g. lever)
		TOGGLE,					// Toggles the global switch (e.g. button)
		UNLOCK,					// Event is unlocked if switch is on
		LOCK					// Event is locked if switch is on
	};

	struct GlobalSwitchProperty : public Property
	{
		std::string searchKey = "";
		uint id = 0;
		GlobalSwitchOnInteract functionOnInteract = GlobalSwitchOnInteract::NONE;

		void ReadProperty(pugi::xml_node const& node) override
		{
			auto propertiesNode = node.child("properties");

			for (auto const& child : propertiesNode.children("property"))
			{
				auto attributeName = child.attribute("name").as_string();
				if (StrEquals("SwitchName", attributeName))
				{
					searchKey = child.attribute("value").as_string();
				}
				else if (StrEquals("ID", attributeName))
				{
					id = child.attribute("value").as_int();
				}
				else if (StrEquals("Function", attributeName))
				{
					functionOnInteract = static_cast<GlobalSwitchOnInteract>(child.attribute("value").as_int());
				}
			}
		}
	};

	struct DestinationProperty : public Property
	{
		std::string destinationMap = "";
		uPoint destination{ 0, 0 };

		void ReadProperty(pugi::xml_node const& node) override
		{
			auto propertiesNode = node.child("properties");

			for (auto const& child : propertiesNode.children("property"))
			{
				auto attributeName = child.attribute("name").as_string();
				if (StrEquals("Destination Map", attributeName))
				{
					destinationMap = child.attribute("value").as_string();
				}
				else if (StrEquals("Destination X", attributeName))
				{
					destination.x = child.attribute("value").as_int();
				}
				else if (StrEquals("Destination Y", attributeName))
				{
					destination.y = child.attribute("value").as_int();
				}
			}
		}
	};

	struct FadeProperty
	{
		SDL_Color fadeColor = { 0, 0, 0, 255 };
	};

}

class Event_Base
{
public:
	Event_Base() = default;
	virtual ~Event_Base() = default;
	virtual void parseXMLProperties(pugi::xml_node const& node) = 0;
	virtual void Create(pugi::xml_node const &node) = 0;

	void Initialize(pugi::xml_node const &node)
	{
		name = node.attribute("name").as_string();

		if (auto propertiesNode = node.child("properties");
			!propertiesNode.empty())
		{
			parseXMLProperties(propertiesNode);
		}
	};

	virtual bool IsEventActive() const
	{
		return common.isActive;
	}

	std::string name = "";
	EventProperties::CommonProperties common;
};

#endif // __EVENT_BASE_H__
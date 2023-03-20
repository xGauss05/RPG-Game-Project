#ifndef __EVENT_TELEPORT_H__
#define __EVENT_TELEPORT_H__

#include "Event_Base.h"
#include "Transform.h"

#include <memory>

class Event_Teleport : public Event_Base, public Transform
{
public:
	// Inherited via Scene_Base
	void parseXMLProperties(pugi::xml_node const& node) override;
	void Create(pugi::xml_node const& node) override;
	// No longer inherited

private:
	EventProperties::DestinationProperty destination;
	std::vector<std::unique_ptr<EventProperties::GlobalSwitchProperty>> globalSwitch;
};

#endif __EVENT_TELEPORT_H__
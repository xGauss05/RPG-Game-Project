#ifndef __EVENT_LEVER_H__
#define __EVENT_LEVER_H__

#include "Event_Base.h"
#include "Sprite.h"

#include <memory>

class Event_Lever : public Event_Base, public Sprite
{
public:
	// Inherited via Scene_Base
	void parseXMLProperties(pugi::xml_node const& node) override;
	// No longer inherited
	EventTrigger OnTrigger() override;
	void Create(pugi::xml_node const& node) override;

private:
	std::vector<EventProperties::GlobalSwitchProperty> globalSwitch;

	bool state = false;
};

#endif __EVENT_LEVER_H__
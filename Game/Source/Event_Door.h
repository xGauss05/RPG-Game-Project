#ifndef __EVENT_DOOR_H__
#define __EVENT_DOOR_H__

#include "Event_Base.h"
#include "Sprite.h"
#include "ObserverPattern.h"

#include <memory>

class Event_Door : public Event_Base, public Sprite, public Subscriber
{
public:
	explicit Event_Door(Publisher& pub);

	void parseXMLProperties(pugi::xml_node const& node) override;
	EventTrigger OnTrigger() override;
	void Create(pugi::xml_node const& node) override;

	void UpdateSubscriber(int id, bool s) override;

protected:
	void AttachToGlobalSwitches() override;

private:
	std::vector<EventProperties::GlobalSwitchProperty> globalSwitch;
	std::unordered_map<int, bool> switchesState;
};

#endif //__EVENT_DOOR_H__
#ifndef __EVENT_TORCH_H__
#define __EVENT_TORCH_H__

#include "Event_Base.h"
#include "Sprite.h"
#include "ObserverPattern.h"

#include <memory>

class Event_Torch : public Event_Base, public Sprite, public Subscriber
{
public:
	explicit Event_Torch(Publisher& pub);

	void parseXMLProperties(pugi::xml_node const& node) override;
	EventTrigger OnTrigger() override;
	void Create(pugi::xml_node const& node) override;

	void UpdateSubscriber(int id, bool s) override;
protected:
	void AttachToGlobalSwitches() override;

private:
	std::vector<EventProperties::GlobalSwitchProperty> globalSwitch;
};

#endif __EVENT_TORCH_H__
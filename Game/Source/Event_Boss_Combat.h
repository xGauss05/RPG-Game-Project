#ifndef __EVENT_BOSS_COMBAT_H__
#define __EVENT_BOSS_COMBAT_H__

#include "NPC_Generic.h"

#include <memory>

class Event_Boss_Combat : public NPC_Generic
{
public:
	// Inherited via Scene_Base
	void parseXMLProperties(pugi::xml_node const& node) override;
	// No longer inherited
	EventTrigger OnTrigger() override;
	void Create(pugi::xml_node const& node) override;

private:
	std::vector<EventProperties::GlobalSwitchProperty> globalSwitch;
};

#endif __EVENT_BOSS_COMBAT_H__
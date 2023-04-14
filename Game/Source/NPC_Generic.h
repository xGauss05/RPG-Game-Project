#ifndef __NPC_GENERIC_H__
#define __NPC_GENERIC_H__

#include "Event_Base.h"
#include "Sprite.h"

#include <memory>

class NPC_Generic : public Event_Base, public Sprite
{
public:
	// Inherited via Scene_Base
	void parseXMLProperties(pugi::xml_node const& node) override;
	
	EventTrigger OnTrigger() override;
	void Create(pugi::xml_node const& node) override;

private:
	bool isStatic = false;
	bool hasDialogue = false;
	std::string dialoguePath = "";
};

#endif __NPC_GENERIC_H__
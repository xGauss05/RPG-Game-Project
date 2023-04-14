#ifndef __NPC_GENERIC_H__
#define __NPC_GENERIC_H__

#include "Event_Base.h"
#include "Sprite.h"

#include <memory>

class NPC_Generic : public Event_Base, public Sprite
{
public:
	// Inherited via Scene_Base
	void parseXMLProperties(pugi::xml_node const& node);
	

	void Create(pugi::xml_node const& node);

private:
	bool isStatic = false;
	bool hasDialogue = false;
};

#endif __NPC_GENERIC_H__
#ifndef __MAP_MENU_COMPONENTPARTY_H__
#define __MAP_MENU_COMPONENTPARTY_H__

#include "GameParty.h"

class Map_Menu_ComponentParty
{
public:
	virtual ~Map_Menu_ComponentParty() = default;
	virtual void SetGameParty(GameParty *party)
	{
		playerParty = party;
	}

protected:
	GameParty* playerParty = nullptr;
};

#endif //__MAP_MENU_COMPONENTPARTY_H__

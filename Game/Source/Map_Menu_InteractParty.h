#ifndef __MAP_MENU_INTERACT_PARTY_H__
#define __MAP_MENU_INTERACT_PARTY_H__

#include "GuiMenuList.h"
#include "Map_Menu_ComponentParty.h"

class Map_Menu_InteractParty : public GuiMenuList, public Map_Menu_ComponentParty
{
public:

	Map_Menu_InteractParty() = default;
	explicit Map_Menu_InteractParty(pugi::xml_node const& node);
	~Map_Menu_InteractParty() override = default;

private:

	void HandleLeftButtonClick(int result) override;
	void HandleRightButtonClick() override;
	void InitializeElements() override;

};

#endif //__MAP_MENU_INTERACT_PARTY_H__

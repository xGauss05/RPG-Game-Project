#ifndef __BATTLE_MENU_MAIN_H__
#define __BATTLE_MENU_MAIN_H__

#include "GuiMenuList.h"
#include "Map_Menu_ComponentParty.h"

class Battle_Menu_Main : public GuiMenuList, public Map_Menu_ComponentParty
{
public:
	Battle_Menu_Main() = default;
	explicit Battle_Menu_Main(pugi::xml_node const& node);
	~Battle_Menu_Main() override;

	int GetTextureID() const;

private:
	void HandleLeftButtonClick(int result) override;
	void HandleRightButtonClick() override;
	void InitializeElements() override;

	int textureID = -1;
};

#endif //__BATTLE_MENU_MAIN_H__

#ifndef __MAP_MENU_MAINCATEGORIES_H__
#define __MAP_MENU_MAINCATEGORIES_H__

#include "GuiMenuList.h"

class Map_Menu_MainCategories : public GuiMenuList
{
public:
	Map_Menu_MainCategories() = default;
	explicit Map_Menu_MainCategories(pugi::xml_node const& node);
	~Map_Menu_MainCategories() override = default;

private:
	void HandleLeftButtonClick(int result) override;
	void HandleRightButtonClick() override;
	void InitializeElements() override;

};

#endif //__MAP_MENU_MAINCATEGORIES_H__

#ifndef __MAP_DISPLAY_QUESTLOG_H__
#define __MAP_DISPLAY_QUESTLOG_H__

#include "GuiMenuList.h"
#include "Map_Menu_ComponentParty.h"

class Map_Display_QuestLog : public GuiMenuList, public Map_Menu_ComponentParty
{
public:
	Map_Display_QuestLog() = default;
	explicit Map_Display_QuestLog(pugi::xml_node const& node);
	~Map_Display_QuestLog() override = default;

	void Start() override;

	void SetPlayerParty(GameParty* party);

	void UpdateQuests();
private:
	void HandleLeftButtonClick(int result) override;
	void HandleRightButtonClick() override;
	void InitializeElements() override;

};

#endif //__MAP_DISPLAY_QUESTLOG_H__

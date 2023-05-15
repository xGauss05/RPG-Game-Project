#include "Map_Display_QuestLog.h"

#include "Render.h"

#include "Log.h"

Map_Display_QuestLog::Map_Display_QuestLog(pugi::xml_node const& node)
	: GuiMenuList(node)
{}

void Map_Display_QuestLog::Start()
{
	SetCurrentItemSelected(-1);
	UpdateQuests();
}

void Map_Display_QuestLog::SetPlayerParty(GameParty *party)
{
	playerParty = party;
}

void Map_Display_QuestLog::HandleLeftButtonClick(int result)
{
	SetClickHandled(true);
}

void Map_Display_QuestLog::HandleRightButtonClick()
{
	ResetCurrentItemSelected();
	SetCurrentAlpha(0);
	SetDeleteMenu(true);
	SetClickHandled(true);
}

void Map_Display_QuestLog::InitializeElements()
{
	UpdateQuests();
}

void Map_Display_QuestLog::UpdateQuests()
{
	ClearMenuItems();

	for (int i = 0; auto const& [id, quest] : playerParty->currentQuests)
	{
		auto displayInfo = quest->GetQuestDisplayInfo();

		if (displayInfo.empty())
			continue;

		CreateMenuItem(MenuItem(MenuItem::ItemText("", std::string(quest->GetQuestName()), ""), i));

		for (auto const& elem : displayInfo)
		{
			std::string objective = "";
			if (elem.objectiveName.empty())
			{
				std::string itemName = playerParty->dbItems->GetItem(elem.objectiveID).general.name;
				objective = AddSaveData(elem.base, itemName);
			}
			else
			{
				objective = AddSaveData(elem.base, elem.objectiveName);
			}
			auto const& progress = elem.amountToDisplay;

			CreateMenuItem(MenuItem(MenuItem::ItemText(objective, "", progress), i));

			i++;
		}
		CreateMenuItem(MenuItem());
	}
}

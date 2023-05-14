#include "DB_Quests.h"

#include "Defs.h"
#include "Log.h"

#include "PugiXml/src/pugixml.hpp"

DB_Quests::DB_Quests()
{
	pugi::xml_document questsFile;
	if (auto result = questsFile.load_file("data/Quests.xml");
		!result)
	{
		LOG("Could not load quests xml file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node questListNode = questsFile.child("quests_list");

	for (auto const& questNode : questListNode.children("quest"))
	{
		int currentQuestID = questNode.attribute("id").as_int();

		auto [currentBuildingQuest, success] =
			availableQuests.try_emplace(
				currentQuestID,
				std::make_unique<Quest>(questNode)
			);

		if (!success)
		{
			LOG("Quest ID %i already exists.", questNode.attribute("id").as_int());
			continue;
		}

		for (auto const& questTypeNode : questNode.children("questtype"))
		{
			auto questBranchType = MapStrToEnum(questTypeNode.attribute("type").as_string());

			if (questBranchType == QuestType::UNKNOWN || questBranchType == QuestType::LAST)
			{
				LOG("Quest with ID %i has invalid type.", currentQuestID);
				continue;
			}

			availableQuests[currentQuestID]->AddBranch(questBranchType);

			for (auto const& questObjectivesNode : questTypeNode.children("objective"))
			{
				availableQuests[currentQuestID]->AddLeafToLastBranch(questObjectivesNode);
			}
		}

	}
}

QuestType DB_Quests::ParseQuestTypeXML(pugi::xml_attribute& node) const
{
	QuestType returnType = QuestType::UNKNOWN;
	returnType |= MapStrToEnum(node.as_string());
	node = node.next_attribute();
	if(node)
	{
		returnType |= ParseQuestTypeXML(node);
	}
	
	return returnType;
}

QuestType DB_Quests::MapStrToEnum(std::string_view str) const
{
	using enum QuestType;

	QuestType returnType = UNKNOWN;

	if (StrEquals("TalkTo", str))	returnType |= TALK_TO;
	if (StrEquals("Collect", str))	returnType |= COLLECT;
	if (StrEquals("Visit", str))	returnType |= VISIT;
	if (StrEquals("Hunt", str))		returnType |= HUNT;

	return returnType;
}

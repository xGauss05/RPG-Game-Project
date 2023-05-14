#ifndef __DB_QUESTS_H__
#define __DB_QUESTS_H__

#include "Quest.h"

#include <vector>
#include <map>
#include <unordered_map>

#include <bitset>

class DB_Quests
{
public:
	DB_Quests();

	// Extracts quest with ID = id and returns the node handle
	inline decltype(auto) QuestAccepted(int id) {
		return availableQuests.extract(id);
	}

private:
	QuestType ParseQuestTypeXML(pugi::xml_attribute& node) const;
	QuestType MapStrToEnum(std::string_view str) const;

	//using QuestMap = std::unordered_map<QuestType, std::unordered_map<int, std::unique_ptr<Quest>>>;
	
	std::unordered_map<int, std::unique_ptr<Quest>> availableQuests;
};

#endif //__DB_QUESTS_H__


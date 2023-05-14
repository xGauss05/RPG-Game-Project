#ifndef __DB_QUESTS_H__
#define __DB_QUESTS_H__

#include "Quest.h"

#include <vector>

#include <unordered_map>
#include <unordered_set>

#include <bitset>

class DB_Quests
{
public:
	DB_Quests();

private:
	QuestType ParseQuestTypeXML(pugi::xml_attribute& node) const;
	QuestType MapStrToEnum(std::string_view str) const;

	std::unordered_map<int, std::unique_ptr<Quest>> availableQuests;
};

#endif //__DB_QUESTS_H__


#ifndef __QUEST_H__
#define __QUEST_H__

#include "App.h"
#include <string>

enum class QuestType
{
	TALK,
	COLLECT,
	GET_TO_POINT,
	UNKNOWN
};

class Quest
{
public:
	Quest() = default;
	virtual ~Quest() = default;

	virtual bool Update() = 0;

	std::string name = "";
	std::string description = "";
	QuestType type = QuestType::UNKNOWN;
};

#endif // __QUEST_H__
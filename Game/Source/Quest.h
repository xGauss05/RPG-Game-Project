#ifndef __QUEST_H__
#define __QUEST_H__

#include "Point.h"
#include "Input.h"
#include "Render.h"

enum class QuestType
{
	TALK,
	COLLECT,
	UNKNOWN
};

class Quest
{
public:
	Quest() {};
	Quest(QuestType type) : type(type) {}

	virtual bool Update() { return true; }

public:

	QuestType type;
	int id;
	int nextQuestId;
	int reward;
};

#endif // __QUEST_H__
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
	~Quest() {};

	virtual bool Update() { return true; }

public:

	std::string name = "";
	std::string description = "";
	QuestType type;
};

#endif // __QUEST_H__
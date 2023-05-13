#ifndef __QUEST_TALK_H__
#define __QUEST_TALK_H__

#include "Quest.h"

class Quest_Talk : public Quest
{
public:
	Quest_Talk() = default;

	bool Update() override;
};

#endif __QUEST_TALK_H__
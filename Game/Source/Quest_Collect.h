#ifndef __QUEST_COLLECT_H__
#define __QUEST_COLLECT_H__

#include "Quest.h"

class Quest_Collect : public Quest
{
public:
	Quest_Collect() = default;

	bool Update() override;

private:
};

#endif __QUEST_COLLECT_H__
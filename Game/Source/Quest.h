#ifndef __QUEST_H__
#define __QUEST_H__

#include "Point.h"
#include "Input.h"
#include "Render.h"

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
	~Quest() = default;

	bool Update() 
	{ 
		switch (type)
		{
		case QuestType::TALK:

			if (app->input->GetKey(SDL_SCANCODE_U) == KeyState::KEY_DOWN) { return false; }

			break;
		case QuestType::COLLECT:

			break;
		case QuestType::GET_TO_POINT:

			break;
		case QuestType::UNKNOWN:

			break;
		default:
			break;
		}

		return true; 
	}

public:

	std::string name = "";
	std::string description = "";
	QuestType type = QuestType::UNKNOWN;
};

#endif // __QUEST_H__
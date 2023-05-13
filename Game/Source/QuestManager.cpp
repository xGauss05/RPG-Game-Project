#include "App.h"
#include "QuestManager.h"
#include "Quest.h"
#include "Quest_Talk.h"
#include "Quest_Collect.h"
#include "TextManager.h"

#include "Input.h"
#include "Point.h"
#include "Render.h"

#include "Defs.h"
#include "Log.h"

#include "PugiXml/src/pugixml.hpp"

#include <format>

QuestManager::QuestManager() : Module()
{
	name = "scene";
}

// Destructor
QuestManager::~QuestManager() = default;

// Called before render is available
bool QuestManager::Awake(pugi::xml_node & config)
{
	LOG("Loading SceneManager");

	pugi::xml_document questsFile;

	if (auto result = questsFile.load_file("data/Quests.xml"); !result)
	{
		LOG("Could not load map xml file Quests.xml. pugi error: %s", result.description());
		return false;
	}

	for (auto const& quest : questsFile.child("quests_list").children("quest"))
	{
		std::unique_ptr<Quest> questToAdd = nullptr;

		switch ((QuestType)quest.attribute("type").as_int())
		{
		case QuestType::TALK:
			questToAdd = std::make_unique<Quest_Talk>();
			break;
		case QuestType::COLLECT:
			questToAdd = std::make_unique<Quest_Collect>();
			break;
		default:
			break;
		}

		questToAdd->name = quest.attribute("name").as_string();
		questToAdd->description = quest.attribute("description").as_string();
		questToAdd->type = (QuestType)quest.attribute("type").as_int();

		quests.push_back(std::move(questToAdd));
	}

	return true;
}

bool QuestManager::Start()
{
	return true;
}

bool QuestManager::PreUpdate()
{
	return true;
}


bool QuestManager::Pause(int phase)
{
	return true;
}

bool QuestManager::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_P) == KeyState::KEY_DOWN) { ActivateQuest("Find kiko's chest"); }
	if (app->input->GetKey(SDL_SCANCODE_O) == KeyState::KEY_DOWN) { ActivateQuest("Steal kiko's chest"); }

	for (auto const& quest : activeQuests)
	{
		if (!quest->Update())
		{
			DeactivateQuest(quest->name);
			break;
		}
	}

	return true;
}

bool QuestManager::PostUpdate()
{
	if (activeQuests.size() <= 0)
	{
		app->fonts->DrawText("No quests active", TextParameters(0, DrawParameters(0, iPoint(20, 20))));
		return true;
	}

	int i = 0;
	for (auto const& quest : activeQuests)
	{
		app->fonts->DrawText(quest->name, TextParameters(0, DrawParameters(0, iPoint(20, 20 + i * 70))));
		app->fonts->DrawText(quest->description, TextParameters(0, DrawParameters(0, iPoint(20, 50 + i * 70))));
		i++;
	}

	return true;
}

bool QuestManager::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool QuestManager::HasSaveData() const
{
	return true;
}

bool QuestManager::LoadState(pugi::xml_node const& data)
{
	return true;
}

pugi::xml_node QuestManager::SaveState(pugi::xml_node const& data) const
{
	pugi::xml_node node = data;

	return node;
}

void QuestManager::ActivateQuest(std::string name)
{
	for (auto& quest : quests)
	{
		if (StrEquals(quest->name, name))
		{
			for (auto& isActiveCheck : activeQuests)
			{
				if (!StrEquals(isActiveCheck->name, name)) { continue; }

				LOG("Quest already active");
				return;
			}

			if (auto derivedPtr = dynamic_cast<Quest_Talk*>(quest.get())) 
			{
				activeQuests.emplace_back(std::make_unique<Quest_Talk>(*derivedPtr));
			}
			else if (auto derivedPtr = dynamic_cast<Quest_Collect*>(quest.get()))
			{
				activeQuests.emplace_back(std::make_unique<Quest_Collect>(*derivedPtr));
			}
		}
	}
}

void QuestManager::DeactivateQuest(std::string name)
{
	//This sets the variable to compare to then take the element out of the vector. Right now, the name
	auto check = [name](const std::unique_ptr<Quest>& q)
	{
		return q->name == name;
	};

	activeQuests.erase(std::remove_if(activeQuests.begin(), activeQuests.end(), check), activeQuests.end());
}

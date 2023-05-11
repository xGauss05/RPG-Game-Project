#include "App.h"
#include "QuestManager.h"

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
bool QuestManager::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneManager");

	pugi::xml_document questsFile;

	if (auto result = questsFile.load_file("Quests.xml"); !result)
	{
		LOG("Could not load map xml file Quests.xml. pugi error: %s", result.description());
		return false;
	}

	return true;
}

// Called before the first frame
bool QuestManager::Start()
{
	return true;
}

// Called each loop iteration
bool QuestManager::PreUpdate()
{
	return true;
}


bool QuestManager::Pause(int phase)
{
	/*iPoint camera = { app->render->GetCamera().x * -1, app->render->GetCamera().y * -1 };
	app->render->DrawTexture(DrawParameters(pauseMenuBackground, camera));

	if (currentScene->OnPause() == 4) return false;*/

	return true;
}

// Called each loop iteration
bool QuestManager::Update(float dt)
{
	// Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	//if (app->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN) 
	//{
	//	LOG("Save Game requested");
	//	app->SaveGameRequest();
	//}

	//if (app->input->GetKey(SDL_SCANCODE_F6) == KeyState::KEY_DOWN) 
	//{
	//	LOG("Load Game requested");
	//	app->LoadGameRequest();
	//}

	//if (app->input->GetKey(SDL_SCANCODE_F7) == KeyState::KEY_DOWN)
	//{
	//	LOG("Minimizing screen");
	//	SDL_SetWindowFullscreen(app->win->GetWindow(), 0);
	//}

	//if (app->input->GetKey(SDL_SCANCODE_F8) == KeyState::KEY_DOWN)
	//{
	//	LOG("Maximizing screen");
	//	SDL_SetWindowFullscreen(app->win->GetWindow(), 1);
	//}

	//currentScene->Draw();

	//using enum TransitionScene;
	//switch (currentScene->Update()) 
	//	{
	//	case BOOT_COMPLETE:
	//		nextScene = std::make_unique<Scene_Title>();
	//		nextScene.get()->Load(assetPath + "UI/", sceneInfo, *windowFactory);
	//		break;
	//	case LOSE_BATTLE:
	//		sceneOnHold.reset();
	//		/*for (auto& character : party->party)
	//		{
	//			character.SetCurrentHP(1);
	//		}*/
	//		nextScene = std::make_unique<Scene_GameOver>();
	//		nextScene.get()->Load(assetPath + "UI/", gameOverInfo, *windowFactory);
	//		break;
	//	case MAIN_MENU:
	//		nextScene = std::make_unique<Scene_Title>();
	//		nextScene.get()->Load(assetPath + "UI/", sceneInfo, *windowFactory);
	//		break;
	//	case NEW_GAME:
	//		nextScene = std::make_unique<Scene_Map>();
	//		nextScene->Load(assetPath + "Maps/", mapInfo, *windowFactory);
	//		nextScene->Start();
	//		break;
	//	case CONTINUE_GAME:
	//	{
	//		nextScene = std::make_unique<Scene_Map>();
	//		loadNextMap = true;
	//		app->LoadGameRequest();
	//		break;
	//	}
	//	case LOAD_MAP_FROM_MAP:
	//	{
	//		auto const* mapScene = dynamic_cast<Scene_Map*>(currentScene.get());
	//		nextScene = std::make_unique<Scene_Map>(std::string(mapScene->GetNextMap()), mapScene->GetTPCoordinates());
	//		nextScene->Load(assetPath + "Maps/", mapInfo, *windowFactory);
	//		nextScene->Start();
	//		break;
	//	}
	//	case START_BATTLE:
	//		StartBattle();
	//		break;
	//	case WIN_BATTLE:
	//	case RUN_BATTLE:
	//		nextScene = std::move(sceneOnHold);
	//		nextScene->isReady(); //Re plays music
	//		break;
	//	case EXIT_GAME:
	//		return false;
	//	case NONE:
	//		break;
	//}

	return true;
}

// Called each loop iteration
bool QuestManager::PostUpdate()
{
	/*if (app->input->GetKey(SDL_SCANCODE_B) == KeyState::KEY_DOWN)
	{
		StartBattle();
	}

	if (nextScene && nextScene->isReady() && !loadNextMap)
	{
		currentScene = std::move(nextScene);
		currentScene->Start();
	}*/

	return true;
}

// Called before quitting
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
	/*pugi::xml_node node = data.child("party_member");

	for (auto& character : party->party)
	{
		character.SetLevel(data.attribute("level").as_int());
		character.SetCurrentHP(data.attribute("currentHP").as_int());
		character.SetCurrentMana(data.attribute("currentMP").as_int());
		character.SetCurrentXP(data.attribute("currentXP").as_int());

		node.next_sibling("party_member");
	}

	currentScene->LoadScene(data);
	if (nextScene)
	{
		nextScene->LoadScene(data);
	}*/

	return true;
}

pugi::xml_node QuestManager::SaveState(pugi::xml_node const& data) const
{
	pugi::xml_node node = data;
	/*
	node = node.append_child("scene");

	currentScene->SaveScene(data);

	for (auto const& character : party->party)
	{

		auto currentNode = node.append_child("party_member");
		currentNode.append_attribute("name").set_value(character.name.c_str());
		currentNode.append_attribute("level").set_value(character.level);
		currentNode.append_attribute("currentHP").set_value(character.currentHP);
		currentNode.append_attribute("currentMP").set_value(character.currentMana);
		currentNode.append_attribute("currentXP").set_value(character.currentXP);

	}*/

	return node;
}

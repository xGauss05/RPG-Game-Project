#include "App.h"
#include "Window.h"
#include "SceneManager.h"
#include "Input.h"

#include "Scene_Map.h"
#include "Scene_Title.h"
#include "Scene_Battle.h"
#include "Scene_Boot.h"

#include "Render.h"
#include "Defs.h"
#include "Log.h"

#include "PugiXml/src/pugixml.hpp"

#include <format>

SceneManager::SceneManager() : Module()
{
	name = "scene";
}

// Destructor
SceneManager::~SceneManager()
{
	app->tex->Unload(pauseMenuBackground);
}

// Called before render is available
bool SceneManager::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneManager");

	assetPath = config.child("general").attribute("assetpath").as_string();

	windowFactory = std::make_unique<Window_Factory>(config);

	for (auto const& node : config.child("title_info").children("scene"))
	{
		sceneInfo[node.attribute("name").as_string()] = node;
	}
	for (auto const& node : config.child("map_info").children("map"))
	{
		mapInfo[node.attribute("name").as_string()] = node;
	}

	party = std::make_unique<GameParty>();

	currentScene = std::make_unique<Scene_Boot>();

	options = false;

	return true;
}

// Called before the first frame
bool SceneManager::Start()
{
	currentScene.get()->Load(assetPath + "UI/", sceneInfo, *windowFactory);
	party->CreateParty();
	pauseMenuBackground = app->tex->Load("Assets/Textures/Backgrounds/pause_bg.png");

	return true;
}

// Called each loop iteration
bool SceneManager::PreUpdate()
{
	if (loadNextMap && nextScene)
	{
		nextScene->Load(assetPath + "Maps/", mapInfo, *windowFactory);
		nextScene->Start();
		loadNextMap = false;
	}
	return true;
}


bool SceneManager::Pause(int phase)
{
	iPoint camera = { app->render->GetCamera().x * -1, app->render->GetCamera().y * -1 };
	app->render->DrawTexture(DrawParameters(pauseMenuBackground, camera));

	if (currentScene->OnPause() == 4) return false;

	return true;
}

// Called each loop iteration
bool SceneManager::Update(float dt)
{
	// Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN) 
	{
		LOG("Save Game requested");
		app->SaveGameRequest();
	}

	if (app->input->GetKey(SDL_SCANCODE_F6) == KeyState::KEY_DOWN) 
	{
		LOG("Load Game requested");
		app->LoadGameRequest();
	}

	if (app->input->GetKey(SDL_SCANCODE_F7) == KeyState::KEY_DOWN)
	{
		LOG("Minimizing screen");
		SDL_SetWindowFullscreen(app->win->GetWindow(), 0);
	}

	if (app->input->GetKey(SDL_SCANCODE_F8) == KeyState::KEY_DOWN)
	{
		LOG("Maximizing screen");
		SDL_SetWindowFullscreen(app->win->GetWindow(), 1);
	}

	currentScene->Draw();

	using enum TransitionScene;
	switch (currentScene->Update()) 
		{
		case BOOT_COMPLETE:
			nextScene = std::make_unique<Scene_Title>();
			nextScene.get()->Load(assetPath + "UI/", sceneInfo, *windowFactory);
			break;
		case LOSE_BATTLE:
			sceneOnHold.reset();
			for (auto& character : party->party)
			{
				character.SetCurrentHP(1);
			}
		case MAIN_MENU:
			nextScene = std::make_unique<Scene_Title>();
			nextScene.get()->Load(assetPath + "UI/", sceneInfo, *windowFactory);
			break;
		case NEW_GAME:
			nextScene = std::make_unique<Scene_Map>();
			nextScene->Load(assetPath + "Maps/", mapInfo, *windowFactory);
			nextScene->Start();
			break;
		case CONTINUE_GAME:
		{
			nextScene = std::make_unique<Scene_Map>();
			loadNextMap = true;
			app->LoadGameRequest();
			break;
		}
		case LOAD_MAP_FROM_MAP:
		{
			auto const* mapScene = dynamic_cast<Scene_Map*>(currentScene.get());
			nextScene = std::make_unique<Scene_Map>(std::string(mapScene->GetNextMap()), mapScene->GetTPCoordinates());
			nextScene->Load(assetPath + "Maps/", mapInfo, *windowFactory);
			nextScene->Start();
			break;
		}
		case START_BATTLE:
			StartBattle();
			break;
		case WIN_BATTLE:
		case RUN_BATTLE:
			nextScene = std::move(sceneOnHold);
			nextScene->isReady(); //Re plays music
			break;
		case EXIT_GAME:
			return false;
		case NONE:
			break;
	}

	return true;
}

// Called each loop iteration
bool SceneManager::PostUpdate()
{
	if (app->input->GetKey(SDL_SCANCODE_B) == KeyState::KEY_DOWN)
	{
		StartBattle();
	}

	if (nextScene && nextScene->isReady() && !loadNextMap)
	{
		currentScene = std::move(nextScene);
		currentScene->Start();
	}

	return true;
}

void SceneManager::StartBattle(std::string_view troopName)
{
	nextScene = std::make_unique<Scene_Battle>(party.get(), troopName);
	nextScene->Load("", sceneInfo, *windowFactory);
	sceneOnHold = std::move(currentScene);
}

// Called before quitting
bool SceneManager::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool SceneManager::HasSaveData() const
{
	return true;
}

bool SceneManager::LoadState(pugi::xml_node const& data)
{
	pugi::xml_node node = data.child("party_member");

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
	}

	return true;
}

pugi::xml_node SceneManager::SaveState(pugi::xml_node const& data) const
{
	pugi::xml_node node = data;
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

	}

	return node;
}

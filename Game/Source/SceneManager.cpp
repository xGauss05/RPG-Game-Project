#include "App.h"
#include "SceneManager.h"
#include "Input.h"

#include "Scene_Map.h"
#include "Scene_Title.h"
#include "Scene_Battle.h"

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

	currentScene = std::make_unique<Scene_Title>();

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
	return true;
}


bool SceneManager::Pause(int phase)
{
	// Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KeyState::KEY_DOWN)
		app->LoadGameRequest();

	app->render->DrawTexture(DrawParameters(pauseMenuBackground, iPoint(0, 0)));

	if(currentScene->OnPause() == 4) return false;

	return true;
}

// Called each loop iteration
bool SceneManager::Update(float dt)
{
	// Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KeyState::KEY_DOWN)
		app->LoadGameRequest();

	currentScene->Draw();

	using enum TransitionScene;
	switch (currentScene->Update()) 
		{
		case BOOT_COMPLETE:
			break;
		case NEW_GAME:
			nextScene = std::make_unique<Scene_Map>();
			break;
		case CONTINUE_GAME:
			break;
		case START_BATTLE:
			//nextScene = std::make_unique<Scene_Battle>(party.get(), combat);
			break;
		case WIN_BATTLE:
		case LOSE_BATTLE:
			break;
		case EXIT_GAME:
			return false;
		case NONE: // tus muertos
			break;
	}
	
	return true;
}

// Called each loop iteration
bool SceneManager::PostUpdate()
{
	if (nextScene && nextScene->isReady())
	{
		currentScene = std::move(nextScene);
		if(CurrentlyMainMenu)
			currentScene.get()->Load(assetPath + "Maps/", mapInfo, *windowFactory);
		else
			currentScene.get()->Load(assetPath + "UI/", sceneInfo, *windowFactory);

		CurrentlyMainMenu = !CurrentlyMainMenu;
	}

	if (app->input->GetKey(SDL_SCANCODE_B) == KeyState::KEY_DOWN)
	{
		pugi::xml_document troopsFile;
		if (auto result = troopsFile.load_file("data/Troops.xml"); !result)
		{
			LOG("Could not load troops xml file. Pugi error: %s", result.description());
		}
		pugi::xml_document enemiesFile;
		if (auto result = enemiesFile.load_file("data/Enemies.xml"); !result)
		{
			LOG("Could not load enemies xml file. Pugi error: %s", result.description());
		}

		EnemyTroops combat;
		for (auto const& enemy : troopsFile.child("basicslime"))
		{
			auto currentEnemy = enemiesFile.child(enemy.name());
			Enemy enemyToAdd;
			enemyToAdd.textureID = app->tex->Load(currentEnemy.child("texture").attribute("path").as_string());
			for (auto const& stat : enemiesFile.child("stats").children())
			{
				enemyToAdd.stats.emplace_back(stat.attribute("value").as_int());
			}
			combat.troop.emplace_back(enemyToAdd);
		}
		nextScene = std::make_unique<Scene_Battle>(party.get(), combat);
	}

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::KEY_DOWN)
		return false;
	
	return true;
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

bool SceneManager::LoadState(pugi::xml_node const &data)
{
	return false;
}

pugi::xml_node SceneManager::SaveState(pugi::xml_node const &data) const
{
	std::string saveData2 = "<{} {}=\"{}\"/>\n";
	std::string saveOpenData2 = "<{} {}=\"{}\">\n";
	std::string saveData4 = "<{} {}=\"{}\" {}=\"{}\"/>\n";
	std::string saveOpenData4 = "<{} {}=\"{}\" {}=\"{}\">\n";
	std::string saveData6 = "<{} {}=\"{}\" {}=\"{}\" {}=\"{}\"/>\n";
	std::string saveData6OneFloat = "<{} {}=\"{}\" {}=\"{}\" {}=\"{}\" {}=\"{:.2f}\"/>\n";
	std::string saveData6OneInt = "<{} {}=\"{}\" {}=\"{:.2f}\" {}=\"{:.2f}\" {}=\"{:.2f}\"/>\n";
	std::string saveFloatData = "<{} {}=\"{:.2f}\" {}=\"{:.2f}\"/>\n";
	std::string dataToSave = "<scene>\n";
	dataToSave += "</scene>";

	app->AppendFragment(data, dataToSave.c_str());

	return data;
}

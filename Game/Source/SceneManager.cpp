#include "App.h"
#include "SceneManager.h"
#include "Input.h"

#include "Scene_Map.h"
#include "Scene_Title.h"

#include "Defs.h"
#include "Log.h"

#include <format>

SceneManager::SceneManager() : Module()
{
	name = "scene";
}

// Destructor
SceneManager::~SceneManager() = default;

// Called before render is available
bool SceneManager::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneManager");

	assetPath = config.child("general").attribute("assetpath").as_string();

	windowFactory = std::make_unique<Window_Factory>(config);

	for (auto const& node : config.child("scene_info").children("scene"))
	{
		sceneInfo[node.attribute("name").as_string()] = node;
	}
	for (auto const& node : config.child("map_info").children("map"))
	{
		mapInfo[node.attribute("name").as_string()] = node;
	}

	currentScene = std::make_unique<Scene_Title>();
	return true;
}

// Called before the first frame
bool SceneManager::Start()
{
	currentScene.get()->Load(assetPath + "UI/", sceneInfo, *windowFactory);

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

	if (currentScene->Update() == 1 || app->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_UP)
	{
		if (CurrentlyMainMenu)
			nextScene = std::make_unique<Scene_Map>();
		else
			nextScene = std::make_unique<Scene_Title>();
	}
	
	return true;
}

// Called each loop iteration
bool SceneManager::PostUpdate()
{
	if (nextScene && nextScene->isReady())
	{
		if(CurrentlyMainMenu)
			nextScene.get()->Load(assetPath + "Maps/", mapInfo, *windowFactory);
		else
			nextScene.get()->Load(assetPath + "UI/", sceneInfo, *windowFactory);

		CurrentlyMainMenu = !CurrentlyMainMenu;

		currentScene = std::move(nextScene);
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

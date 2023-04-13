#include "Scene_Map.h"
#include "Audio.h"
#include "Log.h"

bool Scene_Map::isReady()
{
	return true;
}

void Scene_Map::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFactory)
{
	// Load map
	currentMap = "Base";

	if (std::string mapToLoad = currentMap + ".tmx";
		!map.Load(path, mapToLoad))
	{
		LOG("Map %s couldn't be loaded.", mapToLoad);
	}

	player.Create();

	auto sceneHash = info.find("Map");
	if (sceneHash == info.end())
	{
		LOG("Map scene not found in XML.");
		return;
	}

	auto scene = sceneHash->second;

	for (auto const& window : scene.children("window"))
	{
		if (auto result = windowFactory.CreateWindow(window.attribute("name").as_string());
			result != nullptr)
		{
			std::string s1 = window.attribute("name").as_string();
			int val = s1.compare("PauseButtonMenu");
			if (val == 0) 
			{
				windows.push_back(std::move(result));
			}
			else {
				pauseWindow.push_back(std::move(result));
			}
			/*if (window.attribute("name").as_string() == "PauseButtonMenu") 
			{
				
			}
			else if (window.attribute("name").as_string() == "PauseMenu") 
			{
				pauseWindow.push_back(std::move(result));
			}*/
		}
	}

	app->audio->PlayMusic("Assets/Audio/Music/bgm_placeholder.ogg");
}

void Scene_Map::Start()
{
}

void Scene_Map::Draw()
{
	map.Draw();
	player.Draw();
	for (auto const& elem : windows)
	{
		elem->Draw();
	}
}

int Scene_Map::Update()
{
	auto playerAction = player.HandleInput();

	using PA = Player::PlayerAction::Action;

	if ((playerAction.action & PA::MOVE) == PA::MOVE)
	{
		if (map.IsWalkable(playerAction.destinationTile))
		{
			player.StartAction(playerAction);
		}
	}

	player.Update();

	for (auto const& elem : windows)
	{
		if (auto result = elem->Update();
			result != 0)
			return result;
	}

	return 0;
}

int Scene_Map::OnPause() 
{
	for (auto const& elem : pauseWindow)
	{
		if (auto result = elem->Update();
			result != 0)
			return result;
	}
	for (auto const& elem : pauseWindow)
	{
		elem->Draw();
	}
	return 0;
}

int Scene_Map::CheckNextScene()
{
	return 0;
}

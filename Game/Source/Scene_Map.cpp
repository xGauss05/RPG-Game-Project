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

	this->windowFactory = &windowFactory;
	xmlNode = info;
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
			
			if (StrEquals("PauseButtonMenu", window.attribute("name").as_string()))
			{
				windows.push_back(std::move(result));
			}
			else if (StrEquals("PauseMenu", window.attribute("name").as_string()))
			{
				pauseWindow.push_back(std::move(result));
			}
		}
	}

	app->audio->PlayMusic("Assets/Audio/Music/bgm_placeholder.ogg");
	exit =  false;
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

TransitionScene Scene_Map::Update()
{
	auto playerAction = player.HandleInput();

	using PA = Player::PlayerAction::Action;

	if ((playerAction.action & PA::MOVE) == PA::MOVE && !player.interacting)
	{
		if (map.IsWalkable(playerAction.destinationTile))
		{
			player.StartAction(playerAction);
		}
	}
	else if ((playerAction.action & PA::INTERACT) == PA::INTERACT)
	{
		iPoint checktile = player.GetPosition() + (player.lastDir * map.GetTileWidth());

		if (map.IsEvent(checktile) && map.IsNPC(checktile))
		{
			if (!player.interacting)
			{
				//Do interaction
				player.interacting = true;

				//This loading should not go here, or at least not this way. WIP
				auto sceneHash = xmlNode.find("Map");
				if (sceneHash == xmlNode.end())
				{
					LOG("Map scene not found in XML.");
					return TransitionScene::NONE;
				}

				auto scene = sceneHash->second;

				for (auto const& window : scene.children("window"))
				{
					if (StrEquals("ChatBox", window.attribute("name").as_string()))
					{
						if (auto result = windowFactory->CreateWindow(window.attribute("name").as_string());
							result != nullptr)
						{
							windows.push_back(std::move(result));
						}
					}
				}
			}
			else
			{
				//Remove windows
				player.interacting = false;
				windows.pop_back();
			}
		}
	}

	player.Update();
	
	using enum TransitionScene;
	for (auto const& elem : windows)
	{
		switch (elem->Update())
		{
		case 1:
			return NEW_GAME;
		case 2:
			return CONTINUE_GAME;
		case 3:
			// Create new options window
			break;
		case 4:
			return EXIT_GAME;
		default:
			continue;
		}
	}

	return TransitionScene::NONE;
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

#include "Scene_Map.h"

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
					return -1;
				}

				auto scene = sceneHash->second;

				for (auto const& window : scene.children("window"))
				{
					if (auto result = windowFactory->CreateWindow(window.attribute("name").as_string());
						result != nullptr)
					{
						windows.push_back(std::move(result));
					}
				}
			}
			else
			{
				//Remove windows
				player.interacting = false;
				windows.clear();
			}
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

int Scene_Map::CheckNextScene()
{
	return 0;
}

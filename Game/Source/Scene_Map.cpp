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

	player.Create();
}

void Scene_Map::Start()
{
}

void Scene_Map::Draw()
{
	map.Draw();
	player.Draw();
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

	return 0;
}

int Scene_Map::CheckNextScene()
{
	return 0;
}

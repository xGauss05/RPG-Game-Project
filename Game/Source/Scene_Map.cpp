#include "Scene_Map.h"

#include "Log.h"

bool Scene_Map::isReady()
{
	return false;
}

void Scene_Map::Load(std::string const& path)
{
	// Load map
	currentMap = "Base";

	if (std::string mapToLoad = currentMap + ".tmx";
		!map.Load(path, mapToLoad))
	{
		LOG("Map %s couldn't be loaded.", mapToLoad);
	}
}

void Scene_Map::Start()
{
}

void Scene_Map::Update()
{
	map.Draw();
}

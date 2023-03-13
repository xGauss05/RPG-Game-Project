#include "Map.h"
#include "App.h"
#include "Render.h"

#include "Log.h"

#include "SDL_image/include/SDL_image.h"

Map::Map()
{
}

// Destructor
Map::~Map() = default;

bool Map::Load(const std::string& directory, const std::string& level)
{
	pugi::xml_document mapFile;

	if (auto result = mapFile.load_file((directory + level).c_str()); !result)
	{
		LOG("Could not load map xml file %s. pugi error: %s", directory + level, result.description());
		return false;
	}

	pugi::xml_node map = mapFile.child("map");

	if (!map)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		return false;
	}

	if (StrEquals(map.attribute("orientation").as_string(),"orthogonal"))
		orientation = MapTypes::MAPTYPE_ORTHOGONAL;
	else if (StrEquals(map.attribute("orientation").as_string(),"isometric"))
		orientation = MapTypes::MAPTYPE_ISOMETRIC;
	else if (StrEquals(map.attribute("orientation").as_string(), "staggered"))
		orientation = MapTypes::MAPTYPE_STAGGERED;
	else
	{
		LOG("Error parsing xml file: 'orientation' attribute is unknown.");
		return false;
	}

	size.x = map.attribute("height").as_uint();
	size.y = map.attribute("width").as_uint();
	tileSize.x = map.attribute("tileheight").as_uint();
	tileSize.y = map.attribute("tilewidth").as_uint();

	for (auto const& child : mapFile.child("map"))
	{
		if (StrEquals(child.name(), "tileset"))
		{
			tilesets.emplace_back(child, directory);
		}
		else if (StrEquals(child.name(), "layer"))
		{
			tileLayers.emplace_back(child);
		}
		else if (StrEquals(child.name(), "objectgroup"))
		{
			objectLayers.emplace_back(child);
		}
		else
		{
			LOG("Error parsing xml file: '%s' tag not implemented.", child.name());
		}
	}
	return true;
}

void Map::Draw() const
{
	for (MapLayer layer : tileLayers)
	{
		for (uint x = 0; x < layer.GetSize().x; x++)
		{
			for (uint y = 0; y < layer.GetSize().y; y++)
			{
				uint gid = layer.GetTileGid(x, y);
				
				if (gid <= 0) continue;

				uint tileset = 0;
				while (tileset < tilesets.size() && !tilesets[tileset].ContainsGid(gid))
					tileset++;

				if (tileset >= tilesets.size())
				{
					LOG("Tileset for tile gid %s not found.", gid);
					continue;
				}
				SDL_Rect r = tilesets[tileset].GetTileRect(gid);
				uPoint pos = MapToWorld(x, y);

				app->render->DrawTexture(tilesets[tileset].GetTexture(), pos.x, pos.y, &r);
			}
		}
	}
}

// Translates x,y coordinates from map positions to world positions
uPoint Map::MapToWorld(uint x, uint y) const
{
	return { x * tileSize.x, y * tileSize.y };
}

// Translates x coordinates from map positions to world positions
uint Map::MapXToWorld(uint x) const
{
	return x * tileSize.x;
}

// Translates x,y coordinates from map positions to world positions
uPoint Map::MapToWorld(uPoint position) const
{
	return { position.x * tileSize.x, position.y * tileSize.y };
}

int Map::GetWidth() const { return size.x; };
int Map::GetHeight() const { return size.y; };
int Map::GetTileWidth() const { return tileSize.x; };
int Map::GetTileHeight() const { return tileSize.y; };
int Map::GetTileSetSize() const { return tilesets.size(); };

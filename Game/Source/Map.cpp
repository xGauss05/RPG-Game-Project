#include "Map.h"
#include "App.h"
#include "Render.h"
#include "TextManager.h"

#include "Log.h"

#include "SDL_image/include/SDL_image.h"

#include <ranges>


Map::Map() = default;

// Destructor
Map::~Map()
{
	for (auto const &elem : tilesets)
	{
		elem.Unload();
	}
}

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

	size.x = map.attribute("height").as_int();
	size.y = map.attribute("width").as_int();
	tileSize.x = map.attribute("tileheight").as_int();
	tileSize.y = map.attribute("tilewidth").as_int();

	for (auto const& child : mapFile.child("map"))
	{
		if (StrEquals(child.name(), "tileset"))
		{
			tilesets.emplace_back(child, directory);
		}
		else if (StrEquals(child.name(), "layer"))
		{
			drawOrder.emplace_back(LayerType::TILE_LAYER, tileLayers.size());
			tileLayers.emplace_back(child);
		}
		else if (StrEquals(child.name(), "objectgroup"))
		{
			// If the name of the ObjectGroup is Events use 
			// the EventManager to create and store them
			if (StrEquals("Layer of Events", child.attribute("class").as_string()))
			{
				drawOrder.emplace_back(LayerType::EVENT_LAYER, eventManager.GetEventLayerSize());
				eventManager.CreateEvent(child);
			}
			else 
			{
				drawOrder.emplace_back(LayerType::OBJECT_LAYER, objectLayers.size());
				objectLayers.emplace_back(child);
			}
		}
		else
		{
			LOG("Error parsing xml file: '%s' tag not implemented.", child.name());
			continue;
		}
	}

	eventManager.Initialize();

	return true;
}

void Map::Draw()
{
	for (auto const& [type, index] : drawOrder)
	{
		using enum LayerType;
		switch (type)
		{
			case TILE_LAYER:
				DrawTileLayer(tileLayers[index]);
				break;
			case EVENT_LAYER:
				while(DrawObjectLayer(index));
				break;
			case OBJECT_LAYER:
				break;
		}
	}
}

bool Map::DrawObjectLayer(int index)
{
	auto [gid, pos, keepDrawing] = eventManager.GetDrawEventInfo(index);
	
	if (gid > 0) DrawTile(gid, pos);

	return keepDrawing;
}

void Map::DrawTileLayer(const MapLayer& layer) const
{
	for (int x = 0; x < layer.GetSize().x; x++)
	{
		for (int y = 0; y < layer.GetSize().y; y++)
		{
			int gid = layer.GetTileGid(x, y);

			if (gid == 0) continue;

			DrawTile(gid, MapToWorld(x, y));
		}
	}
}

void Map::DrawTile(int gid, iPoint pos) const
{
	auto result = std::ranges::find_if(
		tilesets,
		[gid](const TileSet& t) { return t.ContainsGid(gid); }
	);

	if (result == tilesets.end())
	{
		LOG("Tileset for tile gid %s not found.", gid);
		return;
	}

	SDL_Rect r = (*result).GetTileRect(gid);

	app->render->DrawTexture(
		DrawParameters((*result).GetTextureID(), iPoint(pos.x, pos.y))
		.Section(&r)
	);
}

// Translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
	return { x * tileSize.x, y * tileSize.y };
}

// Translates x coordinates from map positions to world positions
int Map::MapXToWorld(int x) const
{
	return x * tileSize.x;
}

// Translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(iPoint position) const
{
	return { position.x * tileSize.x, position.y * tileSize.y };
}

iPoint Map::WorldToMap(iPoint position) const
{
	return { position.x / tileSize.x, position.y / tileSize.y };
}

bool Map::IsWalkable(iPoint pos) const
{
	pos = WorldToMap(pos);

	for (auto const &layer : tileLayers)
	{
		auto gid = layer.GetTileGid(pos.x, pos.y);

		if (gid == 0) continue;

		auto result = std::ranges::find_if(
			tilesets,
			[gid](const TileSet &t) { return t.ContainsGid(gid); }
		);

		if (result == tilesets.end())
		{
			LOG("Tileset for tile gid %s not found.", gid);
			return false;
		}

		if (!result->IsWalkable(gid))
			return false;
	}

	return true;
}

int Map::GetWidth() const { return size.x; }
int Map::GetHeight() const { return size.y; }
int Map::GetTileWidth() const { return tileSize.x; }
int Map::GetTileHeight() const { return tileSize.y; }
int Map::GetTileSetSize() const { return tilesets.size(); }

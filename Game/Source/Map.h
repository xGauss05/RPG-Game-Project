#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "TileSet.h"
#include "MapLayer.h"
#include "ObjectLayer.h"
#include "EventManager.h"

#include "Defs.h"

#include <variant>				//std::variant
#include <unordered_map>		//std::unordered_map
#include <vector>				//std::vector

#include "PugiXml/src/pugixml.hpp"

enum class MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

enum class LayerType
{
	TILE_LAYER,
	EVENT_LAYER,
	OBJECT_LAYER
};

class Map
{
public:

	Map();

	// Destructor
	~Map();

	bool Load(const std::string& directory, const std::string& level);

	iPoint MapToWorld(int x, int y) const;

	int MapXToWorld(int x) const;

	iPoint MapToWorld(iPoint position) const;

	iPoint WorldToMap(iPoint position) const;
	
	// Called each loop iteration
	void Draw();
	bool DrawObjectLayer(int index);
	void DrawTileLayer(const MapLayer& layer) const;
	void DrawTile(int gid, iPoint pos) const;
	
	int GetWidth() const;
	int GetHeight() const;

	int GetTileWidth() const;
	int GetTileHeight() const;

	int GetTileSetSize() const;

	bool IsWalkable(iPoint pos) const;

private:
	std::vector<TileSet> tilesets;
	std::vector<MapLayer> tileLayers;
	std::vector<ObjectLayer> objectLayers;
	EventManager eventManager;

	std::vector<std::pair<LayerType, int>> drawOrder;

	MapTypes orientation = MapTypes::MAPTYPE_UNKNOWN;

	iPoint size = { 0, 0 };
	iPoint tileSize = { 0, 0 };
};

#endif // __MAP_H__

#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "TileSet.h"
#include "MapLayer.h"
#include "ObjectLayer.h"

#include "Defs.h"

#include <variant>				//std::variant
#include <unordered_map>		//std::unordered_map
#include <vector>				//std::vector

#include "PugiXml/src/pugixml.hpp"

using XML_Property_t = std::variant<int, bool, float, std::string>;
using XML_Properties_Map_t = std::unordered_map<std::string, XML_Property_t, StringHash, std::equal_to<>>;

enum class MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

class Map
{
public:

	Map();

	// Destructor
	~Map();

	bool Load(const std::string& directory, const std::string& level);

	uPoint MapToWorld(uint x, uint y) const;

	uint MapXToWorld(uint x) const;

	uPoint MapToWorld(uPoint position) const;
	
	// Called each loop iteration
	void Draw() const;

	int GetWidth() const;
	int GetHeight() const;

	int GetTileWidth() const;
	int GetTileHeight() const;

	int GetTileSetSize() const;

private:
	std::vector<TileSet> tilesets;
	std::vector<MapLayer> tileLayers;
	std::vector<ObjectLayer> objectLayers;

	MapTypes orientation = MapTypes::MAPTYPE_UNKNOWN;

	uPoint size = { 0, 0 };
	uPoint tileSize = { 0, 0 };
};

#endif // __MAP_H__

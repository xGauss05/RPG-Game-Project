#pragma once

#include "Point.h"
#include "Defs.h"

#include <vector>

#include "PugiXml/src/pugixml.hpp"
#include "SDL_image/include/SDL_image.h"

enum class MapOrientation
{
	UNKNOWN,
	ORTHOGONAL,
	ISOMETRIC,
	STAGGERED
};

class TileSet
{
public:
	explicit TileSet(const pugi::xml_node& node, const std::string& directory);
	
	void Unload() const;

	uint ContainsGid(uint gid) const;

	SDL_Rect GetTileRect(uint gid) const;

	int GetTextureID() const;

	bool IsWalkable(uint gid) const;

private:
	uint firstGid = 0;
	std::string source = "";
	std::string name = "";
	uPoint tileSize = { 0, 0 };
	uint tileCount = 0;
	uint columns = 0;
	MapOrientation orientation = MapOrientation::UNKNOWN;
	uPoint size = { 0, 0 };
	std::string imageSource = "";
	uPoint sourceSize = { 0, 0 };
	int textureID = -1;

	std::vector<bool> tileWalkability;

	uint margin = 0;
	uint spacing = 0;
};

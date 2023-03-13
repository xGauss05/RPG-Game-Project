#pragma once

#include <vector>

#include "Point.h"
#include "Defs.h"

#include "PugiXml/src/pugixml.hpp"

struct Tile
{
	explicit Tile(uint n)
	{
		const uint FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
		const uint FLIPPED_VERTICALLY_FLAG = 0x40000000;
		const uint FLIPPED_DIAGONALLY_FLAG = 0x20000000;

		if (n & FLIPPED_HORIZONTALLY_FLAG)
			flipFlag += 1;

		if (n & FLIPPED_VERTICALLY_FLAG)
			flipFlag += 2;

		if (n & FLIPPED_DIAGONALLY_FLAG)
			flipFlag += 4;

		gid = n & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);
	};
	uint gid = 0;
	uint flipFlag = 0;
};

class MapLayer
{
public:
	explicit MapLayer(const pugi::xml_node& node);

	uPoint GetSize() const;

	uint GetTileGid(uint x, uint y) const;

private:
	std::string name = "";
	uPoint size = { 0, 0 };
	std::vector<Tile> tiles;
};


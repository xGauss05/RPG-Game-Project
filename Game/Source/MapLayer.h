#ifndef __MAPLAYER_H__
#define __MAPLAYER_H__

#include <vector>

#include "Point.h"
#include "Defs.h"

#include "PugiXml/src/pugixml.hpp"

struct Tile
{
	explicit Tile(int n)
	{
		const int FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
		const int FLIPPED_VERTICALLY_FLAG = 0x40000000;
		const int FLIPPED_DIAGONALLY_FLAG = 0x20000000;

		if (n & FLIPPED_HORIZONTALLY_FLAG)
			flipFlag += 1;

		if (n & FLIPPED_VERTICALLY_FLAG)
			flipFlag += 2;

		if (n & FLIPPED_DIAGONALLY_FLAG)
			flipFlag += 4;

		gid = n & ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);
	};
	int gid = 0;
	int flipFlag = 0;
};

class MapLayer
{
public:
	explicit MapLayer(const pugi::xml_node& node);

	iPoint GetSize() const;

	int GetTileGid(int x, int y) const;

private:
	std::string name = "";
	iPoint size = { 0, 0 };
	std::vector<Tile> tiles;
};

#endif __MAPLAYER_H__
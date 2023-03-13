#include "MapLayer.h"

#include "Log.h"

#include <regex>

MapLayer::MapLayer(const pugi::xml_node& node) :
	name(node.attribute("name").as_string()), 
	size(uPoint{ node.attribute("width").as_uint(), node.attribute("height").as_uint() })
{
	std::string data = node.child("data").text().get();
	
	auto r = std::regex(R"([\d]+)");
	auto textIterator = std::sregex_iterator(data.begin(), data.end(), r);
	auto textIteratorEnd = std::sregex_iterator();

	for (std::sregex_iterator i = textIterator; i != textIteratorEnd; ++i)
	{
		std::smatch match = *i;
		tiles.emplace_back(stoul(match[0].str()));
	}
}

uPoint MapLayer::GetSize() const
{
	return size;
}

uint MapLayer::GetTileGid(uint x, uint y) const
{
	return tiles[(y * size.x) + x].gid;
}
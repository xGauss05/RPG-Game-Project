#include "MapLayer.h"

#include "Log.h"

#include <regex>

MapLayer::MapLayer(const pugi::xml_node& node) :
	name(node.attribute("name").as_string()), 
	size(iPoint{ node.attribute("width").as_int(), node.attribute("height").as_int() })
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

iPoint MapLayer::GetSize() const
{
	return size;
}

int MapLayer::GetTileGid(int x, int y) const
{
	return tiles[(y * size.x) + x].gid;
}
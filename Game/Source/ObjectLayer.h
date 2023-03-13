#pragma once

#include <vector>

#include "Point.h"
#include "Defs.h"

#include "PugiXml/src/pugixml.hpp"

struct Object
{
	uint gid = 0;
	uPoint position = { 0, 0 };
	uPoint size = { 0, 0 };
};

class ObjectLayer
{
public:
	explicit ObjectLayer(const pugi::xml_node& node);

private:
	std::string name = "";
	std::vector<Object> objects;
};


#include "ObjectLayer.h"

#include "Log.h"

#include <regex>

ObjectLayer::ObjectLayer(const pugi::xml_node& node) :
	name(node.attribute("name").as_string())
{
	for (auto const& child : node.children("object"))
	{
		Object obj;
		obj.gid = child.attribute("gid").as_uint();
		obj.position = { child.attribute("x").as_uint(), child.attribute("y").as_uint() };
		obj.size = { child.attribute("width").as_uint(), child.attribute("height").as_uint() };
		objects.push_back(obj);
	}
}

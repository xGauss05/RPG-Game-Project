#ifndef __TRANFORM_MOVABLE_H__
#define __TRANFORM_MOVABLE_H__

#include "Point.h"

#include "PugiXml/src/pugixml.hpp"

class Transform_Movable
{
public:
	iPoint GetPosition() const { return position; };
	iPoint GetSize() const { return size; };

	iPoint position = { 0 };
	iPoint size = { 0 };
protected:
	void Initialize(pugi::xml_node const& node)
	{
		position = { node.attribute("x").as_int(), node.attribute("y").as_int() };
		size = { node.attribute("width").as_int(), node.attribute("height").as_int() };

	}
};


#endif //__TRANFORM_MOVABLE_H__

#ifndef __Window_Factory_H__
#define __Window_Factory_H__

#include "Window_Base.h"
#include "Window_List.h"

#include "PugiXml/src/pugixml.hpp"

class Window_Factory
{
public:
	explicit Window_Factory(pugi::xml_node const& windowInfo);

	std::unique_ptr<Window_Base> CreateWindow(std::string_view const& windowName) const;

private:
	LookUpXMLNodeFromString info;
	int fCleanCraters;
};

#endif __Window_Factory_H__
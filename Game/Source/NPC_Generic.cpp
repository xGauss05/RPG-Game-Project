#include "NPC_Generic.h"

#include "Log.h"

#include <format>
#include <string>

void NPC_Generic::parseXMLProperties(pugi::xml_node const& node)
{
	for (auto const& child : node.children())
	{
		auto attributeName = child.attribute("name").as_string();

		if (StrEquals("Static", attributeName))
		{
			isStatic = child.attribute("value").as_bool();
		}
		else if (StrEquals("Dialogue", attributeName))
		{
			hasDialogue = child.attribute("value").as_bool();
		}
		if (StrEquals("Base", attributeName))
		{
			common.ReadProperty(child);
		}
		else
		{
			LOG("NPC_Base property %s not implemented.", attributeName);
		}
	}
}

void NPC_Generic::Create(pugi::xml_node const& node)
{
	Sprite::Initialize(node);
	Event_Base::Initialize(node);
}
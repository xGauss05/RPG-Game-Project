#include "App.h"
#include "DB_Items.h"
#include "Audio.h"
#include "TextureManager.h"
#include "Log.h"
#include "PugiXml/src/pugixml.hpp"

Item::GeneralProperties::GeneralProperties(pugi::xml_node const& node)
	:
	name(node.child("name").text().as_string()),
	article(node.child("article").text().as_string()),
	iconIndex(node.child("iconIndex").text().as_string()),
	description(node.child("description").text().as_string()),
	iTypeID(node.child("itypeId").text().as_int()),
	price(node.child("price").text().as_int()),
	consumable(node.child("consumable").text().as_bool()),
	scope(static_cast<Scope>(node.child("scope").text().as_int())),
	ocasion(static_cast<Ocasion>(node.child("ocasion").text().as_int()))
{
	if (node.child("sfxpath")) 
	{
		std::string str = node.child("sfxpath").text().as_string();
		str = "Assets/Audio/Fx/" + str;
		itemSfx = app->audio->LoadFx(str);
	}
}

Item::BattleProperties::BattleProperties(pugi::xml_node const& node)
	:
	speed(node.child("speed").text().as_int()),
	successRate(node.child("successRate").text().as_int()),
	repeats(node.child("repeats").text().as_int()),
	hitType(node.child("hitType").text().as_int()),
	animationID(node.child("animationID").text().as_int())
{}

Item::DamageProperties::DamageProperties(pugi::xml_node const& node)
	:
	type(node.child("type").text().as_int()),
	elementID(node.child("elementId").text().as_int()),
	damageFormula(node.child("damageFormula").text().as_string()),
	variance(node.child("variance").text().as_int()),
	canCritical(node.child("critical").text().as_bool())
{}

Item::EffectProperties::EffectProperties(pugi::xml_node const& node)
	:
	functionToCall(node.child("code").text().as_int()),
	dataID(node.child("dataId").text().as_int()),
	param1(node.child("value1").text().as_float()),
	param2(node.child("value2").text().as_float())
{
	for (auto const& elem : node.children("effecttext"))
	{
		text.emplace_back(elem.text().as_string());
	}
}

Item::Item(pugi::xml_node const& itemNode)
	:
	id(itemNode.child("id").text().as_int()),
	general(GeneralProperties(itemNode.child("general"))),
	invocation(BattleProperties(itemNode.child("invocation"))),
	damage(DamageProperties(itemNode.child("damage"))),
	effect(EffectProperties(itemNode.child("effects")))
{
	if (!general.iconIndex.empty())
	{
		textureID = app->tex->Load(std::format("Assets/Textures/Items/{}.png", general.iconIndex));
	}
}

DB_Items::DB_Items()
{
	pugi::xml_document itemsFile;
	if (auto result = itemsFile.load_file("data/Items.xml");
		!result)
	{
		LOG("Could not load items xml file. Pugi error: %s", result.description());
	}

	for (auto const& item : itemsFile.children("item"))
	{
		items.emplace_back(item);
	}
}

int DB_Items::GetItemID(std::string_view itemName) const
{
	for (auto const& item : items)
	{
		if (StrEquals(item.general.name, itemName))
		{
			return item.id;
		}
	}
	return 0;
}

Item const& DB_Items::GetItem(int id) const
{
	for (auto const& item : items)
	{
		if (item.id == id)
		{
			return item;
		}
	}

	return items[0];
}

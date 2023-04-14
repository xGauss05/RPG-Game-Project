#include "App.h"
#include "GameParty.h"
#include "TextureManager.h"
#include "Log.h"
#include "PugiXml/src/pugixml.hpp"


GameParty::GameParty() = default;

void GameParty::CreateParty()
{
	pugi::xml_document battlersFile;
	if (auto result = battlersFile.load_file("data/Characters.xml"); !result)
	{
		LOG("Could not load battlers xml file. pugi error: %s", result.description());
	}
	for (auto const& character : battlersFile.children("character"))
	{
		PartyCharacter memberToAdd;
		memberToAdd.battlerTextureID = app->tex->Load(character.child("texture").attribute("path").as_string());
		for (auto const& stat : character.child("stats").children())
		{
			memberToAdd.stats.emplace_back(stat.attribute("value").as_int());
		}
		party.emplace_back(memberToAdd);
	}
}

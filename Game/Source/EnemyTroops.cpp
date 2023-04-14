#include "App.h"
#include "GameParty.h"
#include "EnemyTroops.h"
#include "TextureManager.h"
#include "Log.h"

void EnemyTroops::CreateFight(std::string const &nodeName)
{
	pugi::xml_document troopsFile;
	if (auto result = troopsFile.load_file("data/Troops.xml"); !result)
	{
		LOG("Could not load troops xml file. Pugi error: %s", result.description());
	}
	pugi::xml_document enemiesFile;
	if (auto result = enemiesFile.load_file("data/Enemies.xml"); !result)
	{
		LOG("Could not load enemies xml file. Pugi error: %s", result.description());
	}

	for (auto const& enemy : troopsFile.child(nodeName.c_str()))
	{
		Enemy enemyToAdd;
		enemyToAdd.name = enemy.name();
		enemyToAdd.level = enemy.attribute("level").as_int();

		auto currentEnemy = enemiesFile.child(enemyToAdd.name.c_str());
		enemyToAdd.textureID = app->tex->Load(currentEnemy.child("texture").attribute("path").as_string());
		for (auto const& stat : currentEnemy.child("stats").children())
		{
			enemyToAdd.stats.emplace_back(stat.attribute("value").as_int());
		}
		troop.emplace_back(enemyToAdd);
	}
}

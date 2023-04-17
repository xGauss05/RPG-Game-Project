#include "App.h"
#include "GameParty.h"
#include "EnemyTroops.h"
#include "TextureManager.h"
#include "Input.h"
#include "Render.h"
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
		iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };

		Enemy enemyToAdd;
		enemyToAdd.name = enemy.name();
		enemyToAdd.level = enemy.attribute("level").as_int();

		auto currentEnemy = enemiesFile.child(enemyToAdd.name.c_str());
		enemyToAdd.textureID = app->tex->Load(currentEnemy.child("texture").attribute("path").as_string());
		for (auto const& stat : currentEnemy.child("stats").children())
		{
			enemyToAdd.stats.emplace_back(stat.attribute("value").as_int());
		}
		enemyToAdd.currentHP = enemyToAdd.stats[0];
		enemyToAdd.currentMana = enemyToAdd.stats[1];

		int w = 0;
		int h = 0;
		app->tex->GetSize(app->GetTexture(enemyToAdd.textureID), w, h);

		enemyToAdd.size.x = w * 2;
		enemyToAdd.size.y = h * 2;

		enemyToAdd.index = troop.size();

		enemyToAdd.position.x = 800 - camera.x;
		enemyToAdd.position.y = 50 + (125 * enemyToAdd.index) - camera.y;

		troop.emplace_back(enemyToAdd);
	}
}

bool Enemy::IsMouseHovering() const
{
	iPoint mousePos = app->input->GetMousePosition();
	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };
	if (mousePos.x >= position.x + camera.x && mousePos.x <= position.x + size.x + camera.x &&
		mousePos.y >= position.y + camera.y && mousePos.y <= position.y + size.y + camera.y)
	{
		return true;
	}

	return false;
}

#include "App.h"
#include "GameParty.h"
#include "EnemyTroops.h"
#include "TextureManager.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Log.h"

EnemyTroops::~EnemyTroops()
{
	for (auto const& elem : troop)
	{
		app->tex->Unload(elem.battlerTextureID);
		app->tex->Unload(elem.portraitTextureID);
	}
}

void EnemyTroops::CreateFight(std::string_view nodeName)
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

	for (auto const& enemy : troopsFile.child(std::string(nodeName).c_str()))
	{
		iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };

		Battler enemyToAdd;
		enemyToAdd.name = enemy.attribute("name").as_string();
		enemyToAdd.level = enemy.attribute("level").as_int();

		auto currentEnemy = enemiesFile.child(enemy.name());
		enemyToAdd.battlerTextureID = app->tex->Load(currentEnemy.child("texture").attribute("path").as_string());
		enemyToAdd.deadSfx = app->audio->LoadFx(currentEnemy.child("deadaudio").attribute("path").as_string());
		for (auto const& stat : currentEnemy.child("stats").children())
		{
			enemyToAdd.AddStat(stat.attribute("value").as_int());
		}
		
		enemyToAdd.currentHP = enemyToAdd.GetStat(BaseStats::MAX_HP);
		enemyToAdd.currentMana = enemyToAdd.GetStat(BaseStats::MAX_MANA);

		int w = 0;
		int h = 0;
		app->tex->GetSize(app->GetTexture(enemyToAdd.battlerTextureID), w, h);

		enemyToAdd.size.x = w * 2;
		enemyToAdd.size.y = h * 2;

		enemyToAdd.index = troop.size();

		enemyToAdd.position.x = 800 - camera.x;
		enemyToAdd.position.y = 50 + (enemyToAdd.size.y * enemyToAdd.index) - camera.y;

		troop.emplace_back(enemyToAdd);
	}
}

bool Battler::IsMouseHovering() const
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

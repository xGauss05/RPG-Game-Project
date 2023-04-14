#include "Scene_Battle.h"
#include "App.h"
#include "Render.h"

Scene_Battle::Scene_Battle(GameParty* gameParty, EnemyTroops enemyTroops)
	: party(gameParty), troop(enemyTroops)
{
}

bool Scene_Battle::isReady()
{
	return true;
}

void Scene_Battle::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFactory)
{
}

void Scene_Battle::Start()
{

}

void Scene_Battle::Draw()
{
	for (int i = 0; auto const& elem : party->party)
	{
		app->render->DrawTexture(
			DrawParameters(elem.battlerTextureID, iPoint(50, 50+(125*i++)))
		);
	}
	for (int i = 0; auto const& elem : troop.troop)
	{
		app->render->DrawTexture(
			DrawParameters(elem.textureID, iPoint(800, 50 + (125 * i++)))
				.Flip(SDL_FLIP_HORIZONTAL)
		);
	}
}

TransitionScene Scene_Battle::Update()
{
	switch (state)
	{
	case BattleState::PLAYER_INPUT:
		break;
	case BattleState::ENEMY_INPUT:
		break;
	case BattleState::RESOLUTION:
		break;
	}

	return TransitionScene::NONE;
}

int Scene_Battle::CheckNextScene()
{
	return 0;
}

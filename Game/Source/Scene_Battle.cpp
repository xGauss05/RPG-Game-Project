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
	windows.clear();
	windows.emplace_back(windowFactory.CreateWindow("BattleActions"));
}

void Scene_Battle::Start()
{

}

void Scene_Battle::Draw()
{
	for (auto const& elem : windows)
	{
		elem->Draw();
	}
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
	using enum BattleState;
	case PLAYER_INPUT:
	{
		switch (windows.front()->Update())
		{
			case 100:
			{
				actionQueue.emplace(0, currentPlayer, 0, true);
				currentPlayer++;
				break;
			}
			case 101:
			{
				actionQueue.emplace(1, currentPlayer, 0, true);
				currentPlayer++;
				break;
			}
			case 102:
			{
				actionQueue.emplace(2, currentPlayer, 0, true);
				currentPlayer++;
				break;
			}
			case 103:
			{
				actionQueue.emplace(3, currentPlayer, 0, true);
				if (currentPlayer == 0)
				{
					currentPlayer = party->party.size();
				}
				break;
			}
			default:
			{

			}
		}

		if (currentPlayer >= party->party.size())
		{
			state = ENEMY_INPUT;
		}
		break;
	}
	case ENEMY_INPUT:
		for (int i = 0; auto const& elem : troop.troop)
		{
			actionQueue.emplace(0, i, 0, false);
		}
		state = RESOLUTION;
		break;
	case RESOLUTION:
		while (!actionQueue.empty())
		{
			BattleAction currentAction = actionQueue.front();
			if (currentAction.friendly)
			{
				if (currentAction.action == 0)
				{
					int attack = party->party[currentAction.source].stats[2];
					int defense = troop.troop[currentAction.target].stats[3];
					if (troop.troop[currentAction.target].isDefending) 
					{
						defense *= 2;
						troop.troop[currentAction.target].isDefending = false;
					}
					int damage = attack - defense;
					troop.troop[currentAction.target].currentHP -= damage;
				}
				if (currentAction.action == 1)
				{
					int attack = party->party[currentAction.source].stats[4];
					int defense = troop.troop[currentAction.target].stats[5];
					if (troop.troop[currentAction.target].isDefending) 
					{
						defense *= 2;
						troop.troop[currentAction.target].isDefending = false;
					}
					int damage = attack - defense;
					troop.troop[currentAction.target].currentHP -= damage;
				}
				else if (currentAction.action == 2)
				{
					party->party[currentAction.target].isDefending = true;
				}
				else
				{
					return TransitionScene::RUN_BATTLE;
				}
			}
			else if (!currentAction.friendly)
			{
				if (currentAction.action == 0)
				{
					int attack = troop.troop[currentAction.source].stats[2];
					int defense = party->party[currentAction.target].stats[3];
					if (party->party[currentAction.target].isDefending)
					{
						defense *= 2;
						party->party[currentAction.target].isDefending = false;
					}
					int damage = attack - defense;
					party->party[currentAction.target].currentHP -= damage;
				}
				else if (currentAction.action == 1)
				{
					int attack = troop.troop[currentAction.source].stats[4];
					int defense = party->party[currentAction.target].stats[5];
					if (party->party[currentAction.target].isDefending)
					{
						defense *= 2;
						party->party[currentAction.target].isDefending = false;
					}
					int damage = attack - defense;
					party->party[currentAction.target].currentHP -= damage;
				}
				else if (currentAction.action == 2)
				{
					troop.troop[currentAction.target].isDefending = true;
				}
			}
		}
		state = PLAYER_INPUT;
		break;
	}

	return TransitionScene::NONE;
}

int Scene_Battle::CheckNextScene()
{
	return 0;
}

int Scene_Battle::OnPause()
{
	return 0;
}

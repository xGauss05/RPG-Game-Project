#include "Scene_Battle.h"
#include "App.h"
#include "Render.h"

Scene_Battle::Scene_Battle(GameParty* gameParty, std::string const &fightName)
	: party(gameParty)
{
	enemies.CreateFight(fightName);
}

bool Scene_Battle::isReady()
{
	return true;
}

void Scene_Battle::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFactory)
{
	windows.clear();
	windows.emplace_back(windowFactory.CreateWindow("BattleActions"));
	//windows.emplace_back(windowFactory.CreateWindow(""))
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
	for (int i = 0; auto const& elem : enemies.troop)
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
		auto actionSpeed = party->party[currentPlayer].stats[static_cast<int>(BaseStats::SPEED)];
		switch (windows.front()->Update())
		{
			case 100:
			{
				actionQueue.emplace(0, currentPlayer, 0, true, actionSpeed);
				currentPlayer++;
				break;
			}
			case 101:
			{
				actionQueue.emplace(1, currentPlayer, 0, true, actionSpeed);
				currentPlayer++;
				break;
			}
			case 102:
			{
				actionQueue.emplace(2, currentPlayer, 0, true, INT_MAX);
				currentPlayer++;
				break;
			}
			case 103:
			{
				actionQueue.emplace(3, currentPlayer, 0, true, INT_MAX);
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
		for (int i = 0; auto const& elem : enemies.troop)
		{
			auto actionSpeed = elem.stats[static_cast<int>(BaseStats::SPEED)];
			actionQueue.emplace(0, i, 0, false, actionSpeed);
			i++;
		}
		state = RESOLUTION;
		break;
	case RESOLUTION:
		while (!actionQueue.empty())
		{
			BattleAction currentAction = actionQueue.top();
			if (currentAction.friendly)
			{
				if (currentAction.action == 0)
				{
					int attack = party->party[currentAction.source].stats[2];
					int defense = enemies.troop[currentAction.target].stats[3];
					if (enemies.troop[currentAction.target].isDefending) 
					{
						defense *= 2;
						enemies.troop[currentAction.target].isDefending = false;
					}
					int damage = attack - defense;
					enemies.troop[currentAction.target].currentHP -= damage;
				}
				else if (currentAction.action == 1)
				{
					int attack = party->party[currentAction.source].stats[4];
					int defense = enemies.troop[currentAction.target].stats[5];
					if (enemies.troop[currentAction.target].isDefending) 
					{
						defense *= 2;
						enemies.troop[currentAction.target].isDefending = false;
					}
					int damage = attack - defense;
					enemies.troop[currentAction.target].currentHP -= damage;
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
					int attack = enemies.troop[currentAction.source].stats[2];
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
					int attack = enemies.troop[currentAction.source].stats[4];
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
					enemies.troop[currentAction.target].isDefending = true;
				}
			}
			actionQueue.pop();
		}
		currentPlayer = 0;
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

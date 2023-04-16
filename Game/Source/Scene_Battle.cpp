#include "Scene_Battle.h"
#include "App.h"
#include "Render.h"

Scene_Battle::Scene_Battle(GameParty* gameParty, std::string const &fightName)
	: party(gameParty)
{
	enemies.CreateFight(fightName);
	for (auto &elem : gameParty->party)
	{
		if (elem.currentHP <= 0) elem.currentHP = elem.stats[0];
	}
}

bool Scene_Battle::isReady()
{
	return true;
}

void Scene_Battle::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFactory)
{
	windows.clear();
	windows.emplace_back(windowFactory.CreateWindow("BattleActions"));
	windows.emplace_back(windowFactory.CreateWindow("Message"));
	//windows.emplace_back(windowFactory.CreateWindow(""))
}

void Scene_Battle::Start()
{

}

void Scene_Battle::DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const
{

	int w = 0;
	int h = 0;
	app->tex->GetSize(app->GetTexture(textureID), w, h);

	SDL_Rect hpBar{};
	hpBar.x = position.x;
	hpBar.y = position.y + h - 10;

	float hp = static_cast<float>(currentHP) / static_cast<float>(maxHP);
	hpBar.w = hp > 0 ? static_cast<int>(hp * 50.0f) : 0;
	hpBar.h = 10;

	app->render->DrawShape(hpBar, true, SDL_Color(255, 0, 0, 255));
}

void Scene_Battle::ChooseTarget()
{
	

	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
	{
		for (auto const& elem : enemies.troop)
		{
			if (elem.IsMouseHovering())
			{
				targetSelected = elem.index;
				break;
			}
		}
	}
}

void Scene_Battle::Draw()
{
	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };

	for (auto const& elem : windows)
	{
		elem->Draw();
	}
	for (int i = 0; auto const& elem : party->party)
	{
		iPoint position(50 - camera.x, 50 + (125 * i) - camera.y);
		DrawHPBar(elem.battlerTextureID, elem.currentHP, elem.stats[0], position);

		app->render->DrawTexture(
			DrawParameters(elem.battlerTextureID, position)
		);
		i++;
	}
	for (int i = 0; auto &elem : enemies.troop)
	{
		DrawHPBar(elem.textureID, elem.currentHP, elem.stats[0], elem.position);

		if (elem.IsMouseHovering() && actionSelected != -1)
		{
			SDL_SetTextureAlphaMod(app->GetTexture(elem.textureID), elem.alpha);
			if (elem.fadingIn)
			{
				elem.alpha += 5;
				if (elem.alpha >= 255)
				{
					elem.fadingIn = false;
					elem.alpha = 255;
				}
			}
			else
			{
				elem.alpha -= 5;
				if (elem.alpha <= 0)
				{
					elem.fadingIn = true;
					elem.alpha = 0;
				}
			}
		}
		else
		{
			elem.alpha = 255;
		}
		app->render->DrawTexture(
			DrawParameters(elem.textureID, elem.position)
				.Flip(SDL_FLIP_HORIZONTAL)
		);
		SDL_SetTextureAlphaMod(app->GetTexture(elem.textureID), 255);
		i++;
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
			if(actionSelected != -1)
			{
				ChooseTarget();
				if(targetSelected != -1)
				{
					actionQueue.emplace(actionSelected, currentPlayer, targetSelected, true, actionSpeed);
					actionSelected = -1;
					targetSelected = -1;
					currentPlayer++;
				}
			}
			else
			{
				switch (windows.front()->Update())
				{
					case 100:
					{
						actionSelected = 0;
						break;
					}
					case 101:
					{
						actionSelected = 1;
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
					{}
				}
			}
			if (actionSelected == -1 && currentPlayer >= party->party.size())
			{
				state = ENEMY_INPUT;
			}
			break;
		}
		case ENEMY_INPUT:
		{
			for (int i = 0; auto const& elem : enemies.troop)
			{
				auto actionSpeed = elem.stats[static_cast<int>(BaseStats::SPEED)];
				actionQueue.emplace(0, i, 0, false, actionSpeed);
				i++;
			}
			state = RESOLUTION;
			break;
		}
		case RESOLUTION:
		{
			if (showNextText)
			{
				BattleAction currentAction = actionQueue.top();
				std::string text = "";
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
						if (damage <= 0) damage = 1;
						enemies.troop[currentAction.target].currentHP -= damage;

						text = std::format("{} attacks {}! Deals {} damage.", party->party[currentAction.source].name, enemies.troop[currentAction.target].name, damage);
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
						if (damage <= 0) damage = 1;
						enemies.troop[currentAction.target].currentHP -= damage;

						text = std::format("{} uses magic on {}! Deals {} damage.", party->party[currentAction.source].name, enemies.troop[currentAction.target].name, damage);
					}
					else if (currentAction.action == 2)
					{
						party->party[currentAction.target].isDefending = true;

						text = std::format("{} is defending.", party->party[currentAction.source].name);
					}
					else
					{
						text = "You run from battle. Lost 124 gold.";
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
						if (damage <= 0) damage = 1;
						party->party[currentAction.target].currentHP -= damage;

						text = std::format("{} attacks {}! Deals {} damage.", enemies.troop[currentAction.target].name, party->party[currentAction.source].name, damage);
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
						if (damage <= 0) damage = 1;
						party->party[currentAction.target].currentHP -= damage;

						text = std::format("{} attacks {}! Deals {} damage.", enemies.troop[currentAction.target].name, party->party[currentAction.source].name, damage);
					}
					else if (currentAction.action == 2)
					{
						enemies.troop[currentAction.target].isDefending = true;

						text = std::format("{} is defending.", enemies.troop[currentAction.target].name);
					}
				}
				dynamic_cast<Window_Panel*>(windows[1].get())->ModifyLastWidgetText(text);
				showNextText = false;
				actionQueue.pop();
			}
			else if (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_DOWN)
			{
				showNextText = true;
			}

			if (actionQueue.empty())
			{
				dynamic_cast<Window_Panel*>(windows[1].get())->ModifyLastWidgetText("");
				currentPlayer = 0;
				state = PLAYER_INPUT;
			}
			break;
		}
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

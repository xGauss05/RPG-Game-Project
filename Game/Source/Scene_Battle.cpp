#include "Scene_Battle.h"
#include "App.h"
#include "Render.h"
#include "Audio.h"
#include "Input.h"

#include "GuiButton.h"

#include "PugiXml/src/pugixml.hpp"

#include <random>

Scene_Battle::Scene_Battle(GameParty* gameParty, std::string_view fightName)
	: party(gameParty)
{
	if (fightName.empty())
		enemies.CreateFight(GetRandomEncounter());
	else
		enemies.CreateFight(fightName);

	if (CheckBattleLoss())
	{
		party->party.front().SetCurrentHP(1);
	}
}

std::string_view Scene_Battle::GetRandomEncounter()
{
	pugi::xml_document encountersFile;
	if (auto result = encountersFile.load_file("data/Troops.xml");
		!result)
	{
		LOG("Could not load troops xml file. Pugi error: %s", result.description());
		return "fallback";
	}

	int maxEncounters = std::distance(encountersFile.children().begin(), encountersFile.children().end()) - 2; // 2 because last fight is boss, not random. fk hardcoding

	random.param(std::uniform_int_distribution<>::param_type(0, maxEncounters));

	std::mt19937 gen(rd());

	int newEncounter = random(gen);

	pugi::xml_node currentNode = encountersFile.first_child();

	for (int i = 0; i < newEncounter; i++, currentNode = currentNode.next_sibling());

	return currentNode.name();
}

bool Scene_Battle::isReady()
{
	return true;
}

void Scene_Battle::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFactory)
{

	windows.clear();

	actions = windowFactory.CreateWindowList("BattleActions");
	messages = windowFactory.CreateWindowPanel("BattleMessage");

	// This produces random values uniformly distributed from 0 to 40 and 1 to 100 respectively
	random40.param(std::uniform_int_distribution<>::param_type(0, 40));
	random100.param(std::uniform_int_distribution<>::param_type(1, 100));

	app->audio->PlayMusic("Music/M_Battle-Main.ogg");
	attackSfx = app->audio->LoadFx("Fx/S_Battle-Attack.wav");
	criticalSfx = app->audio->LoadFx("Fx/S_Battle-AttackCrit.wav");
	blockSfx = app->audio->LoadFx("Fx/S_Battle-Block.wav");
	escapeSfx = app->audio->LoadFx("Fx/S_Battle-Escape.wav");
	erYonaTurnSfx = app->audio->LoadFx("Fx/S_Battle-ErYona-Turn.wav");
	rocioTurnSfx = app->audio->LoadFx("Fx/S_Battle-Rocio-Turn.wav");
	antonioTurnSfx = app->audio->LoadFx("Fx/S_Battle-Antonio-Turn.wav");
	sayoriTurnSfx = app->audio->LoadFx("Fx/S_Battle-Sayori-Turn.wav");
	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/batte_bg.png");
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
	hpBar.x = position.x + 2;
	hpBar.y = position.y + h * 2 + 10;
	hpBar.h = 10;

	hpBar.w = 100;
	app->render->DrawShape(hpBar, true, SDL_Color(0, 0, 0, 255));

	float hp = static_cast<float>(currentHP) / static_cast<float>(maxHP);
	hpBar.w = hp > 0 ? static_cast<int>(hp * 100.0f) : 0;

	auto red = static_cast<Uint8>(250.0f - (250.0f * hp));
	auto green = static_cast<Uint8>(250.0f * hp);

	app->render->DrawShape(hpBar, true, SDL_Color(red, green, 0, 255));
}

bool Scene_Battle::ChooseTarget()
{
	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
	{
		for (auto const& elem : enemies.troop)
		{
			if (elem.IsMouseHovering() && elem.currentHP > 0)
			{
				targetSelected = elem.index;
				return true;
			}
		}
	}

	return false;
}

void Scene_Battle::Draw()
{
	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };

	app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(-camera.x, -camera.y)));

	for (auto const& elem : windows)
	{
		elem->Draw();
	}

	messages->Draw();
	actions->Draw();

	for (int i = 0; auto const& elem : party->party)
	{
		iPoint allyPosition(300 - camera.x, (120 * i) - camera.y + 55);
		iPoint hpBarPosition(270 - camera.x, (120 * i) - camera.y + 80);
		DrawHPBar(elem.battlerTextureID, elem.currentHP, elem.stats[0], hpBarPosition);

		DrawParameters drawAlly(elem.battlerTextureID, allyPosition);

		if (elem.currentHP <= 0)
		{
			drawAlly.RotationAngle(90);

			int w = 0;
			int h = 0;
			app->tex->GetSize(app->GetTexture(elem.battlerTextureID), w, h);

			SDL_Point pivot = {
				w / 2,
				h
			};

			drawAlly.Center(pivot);
		}

		drawAlly.Scale(fPoint{ 3,3 });

		app->render->DrawTexture(drawAlly);
		i++;
	}

	bool enemyHovered = false;

	for (auto& elem : enemies.troop)
	{
		DrawHPBar(elem.textureID, elem.currentHP, elem.stats[0], elem.position);

		if (elem.IsMouseHovering() && actionSelected != -1 && elem.currentHP > 0)
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
			std::string text = std::format("{} | HP: {} | Def: {} | Sp. Def: {}", elem.name, elem.currentHP, elem.stats[3], elem.stats[5]);
			messages->ModifyLastWidgetText(text);

			enemyHovered = true;
		}
		else
		{
			elem.alpha = 255;
		}

		DrawParameters drawEnemy(elem.textureID, elem.position);
		drawEnemy.Flip(SDL_FLIP_HORIZONTAL);
		drawEnemy.Scale(fPoint(2, 2));


		if (elem.currentHP <= 0)
		{
			drawEnemy.RotationAngle(270);

			SDL_Point pivot = {
				elem.size.x / 2,
				elem.size.y
			};

			drawEnemy.Center(pivot);
		}

		app->render->DrawTexture(drawEnemy);
		SDL_SetTextureAlphaMod(app->GetTexture(elem.textureID), 255);
	}

	if (actionSelected != -1 && actionSelected != 3 && !enemyHovered)
	{
		std::string text = "Choose a target.";
		messages->ModifyLastWidgetText(text);
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

		if (currentPlayer != 0)
			actions->widgets.back()->Disable();
		else
			actions->widgets.back()->Enable();

		if (actionSelected == 0 || actionSelected == 1)
		{
			bool targetChosen = ChooseTarget();

			if (targetChosen)
			{
				actionQueue.emplace(actionSelected, currentPlayer, targetSelected, true, actionSpeed);

				actionSelected = -1;
				targetSelected = -1;
				currentPlayer++;

			}
			else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
			{
				actionSelected = -1;
				targetSelected = -1;

			}
		}
		else if (currentPlayer < party->party.size() && party->party[currentPlayer].currentHP > 0)
		{
			std::string text = std::format("What will {} do?", party->party[currentPlayer].name);

			if (!playedTurnSfx)
			{
				playedTurnSfx = true;
				if (StrEquals(party->party[currentPlayer].name, "Antonio")) {
					app->audio->PlayFx(antonioTurnSfx);
				}
				if (StrEquals(party->party[currentPlayer].name, "Sayori")) {
					app->audio->PlayFx(sayoriTurnSfx);
				}
				if (StrEquals(party->party[currentPlayer].name, "Er Yona")) {
					app->audio->PlayFx(erYonaTurnSfx);
				}
				if (StrEquals(party->party[currentPlayer].name, "Rocio")) {
					app->audio->PlayFx(rocioTurnSfx);
				}
			}

			messages->ModifyLastWidgetText(text);
			switch (actions->Update())
			{
			case 100:
			{
				actionSelected = 0;
				dynamic_cast<GuiButton*>(actions->widgets[actions->lastWidgetInteractedIndex].get())->ToggleSelected();
				playedTurnSfx = false;
				break;
			}
			case 101:
			{
				actionSelected = 1;
				dynamic_cast<GuiButton*>(actions->widgets[actions->lastWidgetInteractedIndex].get())->ToggleSelected();
				playedTurnSfx = false;
				break;
			}
			case 102:
			{
				actionQueue.emplace(2, currentPlayer, 0, true, INT_MAX);
				currentPlayer++;
				playedTurnSfx = false;
				break;
			}
			case 103:
			{
				if (currentPlayer == 0)
				{
					actionQueue.emplace(3, currentPlayer, 0, true, INT_MAX);
					currentPlayer = party->party.size();
				}
				break;
			}
			default:
			{}
			}
		}
		else
		{
			currentPlayer++;
		}

		if (actionSelected == -1 && currentPlayer >= party->party.size())
		{
			state = ENEMY_INPUT;
		}
		break;
	}
	case ENEMY_INPUT:
	{
		dynamic_cast<GuiButton*>(actions->widgets[actions->lastWidgetInteractedIndex].get())->ToggleSelected();

		std::mt19937 gen(rd());
		random.param(std::uniform_int_distribution<>::param_type(0, party->party.size() - 1));

		for (int i = 0; auto const& elem : enemies.troop)
		{
			auto actionSpeed = elem.stats[static_cast<int>(BaseStats::SPEED)];

			int target = 0;
			do
			{
				target = random(gen);
			} while (party->party[target].currentHP <= 0);

			int action = random100(gen);
			if (action <= 40)		// 40% chance
			{
				actionQueue.emplace(0, i, target, false, actionSpeed);
			}
			else if (action <= 80)	// 40% chance
			{
				actionQueue.emplace(1, i, target, false, actionSpeed);
			}
			else					//20% chance
			{
				actionQueue.emplace(2, i, target, false, INT_MAX);
			}

			LOG("Target: %i || Action: %i", target, action);

			i++;
		}
		state = RESOLUTION;
		break;
	}
	case RESOLUTION:
	{
		std::mt19937 gen(rd());
		if (!actionQueue.empty() && showNextText && actionSelected != 3)
		{
			BattleAction currentAction = actionQueue.top();
			std::string text = "";
			if (currentAction.friendly)
			{
				if (currentAction.action == 3)
				{
					text = "You run from battle. Shame.";
					app->audio->PlayFx(escapeSfx);
					actionSelected = 3;
				}
				else if (party->party[currentAction.source].currentHP > 0)
				{
					// If defend command selected or enemy is dead
					if (currentAction.action == 2 || enemies.troop[currentAction.target].currentHP <= 0)
					{
						party->party[currentAction.source].isDefending = true;
						text = std::format("{} is defending.", party->party[currentAction.source].name);
						app->audio->PlayFx(blockSfx);
					}
					else if (currentAction.action == 0)
					{
						int attack = party->party[currentAction.source].stats[2];
						int defense = enemies.troop[currentAction.target].stats[3];
						if (enemies.troop[currentAction.target].isDefending)
						{
							defense *= 2;
							enemies.troop[currentAction.target].isDefending = false;
						}

						int randomVariance = random40(gen);
						int damage = attack - defense;
						if (randomVariance > 20)
						{
							randomVariance -= 20;
							damage += static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
						}
						else
						{
							damage -= static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
						}

						std::string damageMessage = "{} attacks {}! Deals {} damage.";

						if (random100(gen) <= 10)
						{
							damage = static_cast<int>(static_cast<float>(damage) * 1.5f);
							damageMessage = "{} attacks {}! Criticals for {} damage!!!";
							app->audio->PlayFx(criticalSfx);
						}
						else
						{
							app->audio->PlayFx(attackSfx);
						}

						if (damage <= 0) damage = 1;

						enemies.troop[currentAction.target].currentHP -= damage;

						if (enemies.troop[currentAction.target].currentHP <= 0)
						{
							app->audio->PlayFx(enemies.troop[currentAction.target].deadSfx);
						}

						text = AddSaveData(damageMessage, party->party[currentAction.source].name, enemies.troop[currentAction.target].name, damage);
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
						int randomVariance = random40(gen);
						int damage = attack - defense;
						if (randomVariance > 20)
						{
							randomVariance -= 20;
							damage += static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
						}
						else
						{
							damage -= static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
						}
						std::string damageMessage = "{} attacks {}! Deals {} damage.";
						if (random100(gen) <= 10)
						{
							damage = static_cast<int>(static_cast<float>(damage) * 1.5f);
							damageMessage = "{} attacks {}! Criticals for {} damage!!!";
							app->audio->PlayFx(criticalSfx);
						}
						else
						{
							app->audio->PlayFx(attackSfx);
						}
						if (damage <= 0) damage = 1;
						enemies.troop[currentAction.target].currentHP -= damage;

						text = AddSaveData(damageMessage, party->party[currentAction.source].name, enemies.troop[currentAction.target].name, damage);
					}
				}

			}
			else if (!currentAction.friendly && enemies.troop[currentAction.source].currentHP > 0)
			{
				if (currentAction.action == 2 || party->party[currentAction.target].currentHP <= 0)
				{
					enemies.troop[currentAction.source].isDefending = true;
					app->audio->PlayFx(blockSfx);
					text = std::format("{} is defending.", enemies.troop[currentAction.source].name);
				}
				else if (currentAction.action == 0)
				{
					int attack = enemies.troop[currentAction.source].stats[2];
					int defense = party->party[currentAction.target].stats[3];
					if (party->party[currentAction.target].isDefending)
					{
						defense *= 2;
						party->party[currentAction.target].isDefending = false;
					}
					int randomVariance = random40(gen);
					int damage = attack - defense;
					if (randomVariance > 20)
					{
						randomVariance -= 20;
						damage += static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
					}
					else
					{
						damage -= static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
					}
					std::string damageMessage = "{} attacks {}! Deals {} damage.";
					if (random100(gen) <= 10)
					{
						damage = static_cast<int>(static_cast<float>(damage) * 1.5f);
						damageMessage = "{} attacks {}! Criticals for {} damage!!!";
						app->audio->PlayFx(criticalSfx);
					}
					else
					{
						app->audio->PlayFx(attackSfx);
					}
					if (damage <= 0) damage = 1;
					party->party[currentAction.target].currentHP -= damage;
					text = AddSaveData(damageMessage, enemies.troop[currentAction.source].name, party->party[currentAction.target].name, damage);
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
					int randomVariance = random40(gen);
					int damage = attack - defense;
					if (randomVariance > 20)
					{
						randomVariance -= 20;
						damage += static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
					}
					else
					{
						damage -= static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
					}
					std::string damageMessage = "{} attacks {}! Deals {} damage.";
					if (random100(gen) <= 10)
					{

						damage = static_cast<int>(static_cast<float>(damage) * 1.5f);
						damageMessage = "{} attacks {}! Criticals for {} damage!!!";
						app->audio->PlayFx(criticalSfx);
					}
					else
					{
						app->audio->PlayFx(attackSfx);
					}
					if (damage <= 0) damage = 1;
					party->party[currentAction.target].currentHP -= damage;

					text = AddSaveData(damageMessage, enemies.troop[currentAction.source].name, party->party[currentAction.target].name, damage);
				}
			}

			if (!text.empty())
			{
				messages->ModifyLastWidgetText(text);
				showNextText = false;
			}
			actionQueue.pop();
		}
		else if (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			showNextText = true;

			if (CheckBattleWin())
			{
				LOG("Battle won.");
				state = BATTLE_WON;
				app->audio->PlayMusic("Music/M_Battle-Win.ogg");
				break;
			}
			if (CheckBattleLoss())
			{
				LOG("Battle loss.");
				state = BATTLE_LOSS;

				break;
			}
			if (actionSelected == 3)
			{
				return TransitionScene::RUN_BATTLE;
			}
		}

		if (showNextText && actionQueue.empty())
		{
			messages->ModifyLastWidgetText("");
			currentPlayer = 0;
			state = PLAYER_INPUT;
		}
		break;
	}
	case BATTLE_WON:
	{
		messages->ModifyLastWidgetText("You won the battle!");
		if (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			messages->ModifyLastWidgetText("");

			std::vector<std::pair<std::string_view, int>> enemiesDefeated;

			for (auto const& enemy : enemies.troop)
			{
				std::string_view enemyName = enemy.name;
				bool enemyFound = false;
				for (auto &[name, amount]: enemiesDefeated)
				{
					if (name == enemyName)
					{
						amount++;
						enemyFound = true;
					}
				}
				if(!enemyFound)
				{
					enemiesDefeated.emplace_back(enemyName, 1);
				}
			}

			party->PossibleQuestProgress(QuestType::HUNT, enemiesDefeated, std::vector<std::pair<int, int>>());

			return TransitionScene::WIN_BATTLE;
		}
		break;
	}
	case BATTLE_LOSS:
	{
		messages->ModifyLastWidgetText("You lost the battle...");
		if (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			messages->ModifyLastWidgetText("");
			return TransitionScene::LOSE_BATTLE;
		}
		break;
	}
	}

	return TransitionScene::NONE;
}

bool Scene_Battle::CheckBattleWin() const
{
	return !std::ranges::any_of(enemies.troop, [](Enemy const& e) { return e.currentHP > 0; });
}

bool Scene_Battle::CheckBattleLoss() const
{
	return !std::ranges::any_of(party->party, [](PartyCharacter const& PC) { return PC.currentHP > 0; });
}

int Scene_Battle::CheckNextScene()
{
	return 0;
}

int Scene_Battle::OnPause()
{
	return 0;
}

bool Scene_Battle::SaveScene(pugi::xml_node const& info)
{
	return false;
}

bool Scene_Battle::LoadScene(pugi::xml_node const& info)
{
	return false;
}

void Scene_Battle::DebugDraw()
{
	return;
}

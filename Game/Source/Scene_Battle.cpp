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
	enemies = std::make_unique<EnemyTroops>();
	if (fightName.empty())
		enemies->CreateFight(GetRandomEncounter());
	else
		enemies->CreateFight(fightName);

	CheckBattleLossThenChangeState();

	SDL_Rect camera = app->render->GetCamera();
	int startingX = 128;
	int startingY = 72;

	for (int i = 0; auto &elem : party->party)
	{
		elem.size = app->tex->GetSize(elem.battlerTextureID);
		elem.size.x /= 4;
		elem.size.y /= 2;
		elem.currentAnimation = { 0, 0, elem.size.x, elem.size.y };
		elem.size =
		{
			static_cast<int>(elem.size.x * 1.25f),
			static_cast<int>(elem.size.y * 1.25f)
		};
		elem.position = {startingX - camera.x, startingY + (elem.size.y * i) - camera.y};
		elem.animTimer = std::chrono::steady_clock::now();
		elem.index = i;
		i++;
	}
}

bool Scene_Battle::isReady()
{
	return true;
}

void Scene_Battle::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFactory)
{
	windows.clear();

	menu = std::make_unique<Battle_Window_Menu>(windowFactory);
	menu->SetPlayerParty(party);
	menu->SetEnemyTroop(enemies.get());
	menu->Start();

	messages.SetPanelArea(SDL_Rect(384, 592, 512, 128));
	messages.SetActive(false);

	// This produces random values uniformly distributed from 0 to 40 and 1 to 100 respectively
	random40.param(std::uniform_int_distribution<>::param_type(0, 40));
	random100.param(std::uniform_int_distribution<>::param_type(1, 100));

	app->audio->PlayMusic("Music/M_Battle-Main.ogg");

	sfx["Antonio"] = app->audio->LoadFx("Fx/S_Battle-Antonio-Turn.wav");
	sfx["Sayori"] = app->audio->LoadFx("Fx/S_Battle-Sayori-Turn.wav");
	sfx["Er Yona"] = app->audio->LoadFx("Fx/S_Battle-ErYona-Turn.wav");
	sfx["Rocio"] = app->audio->LoadFx("Fx/S_Battle-Rocio-Turn.wav");
	sfx["Attack"] = app->audio->LoadFx("Fx/S_Battle-Attack.wav");
	sfx["Critical"] = app->audio->LoadFx("Fx/S_Battle-AttackCrit.wav");
	sfx["Block"] = app->audio->LoadFx("Fx/S_Battle-Block.wav");
	sfx["Run"] = app->audio->LoadFx("Fx/S_Battle-Escape.wav");

	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/batte_bg.png");

	for (auto & elem : enemies->troop)
	{
		elem.SetParticleEmitters(particleDB);
	}
}

Scene_Battle::~Scene_Battle()
{
	LOG("Destroying Scene");
}

void Scene_Battle::Start(){}

void Scene_Battle::Draw()
{
	SDL_Rect camera = app->render->GetCamera();

	app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(-camera.x, -camera.y)));

	if (messages.IsActive())
	{
		messages.Draw();
	}

	std::string text = "";
	auto now = std::chrono::steady_clock::now();

	for (auto & elem : party->party)
	{
		DrawParameters drawAlly(elem.battlerTextureID, elem.position);

		drawAlly
			.Flip(SDL_FLIP_HORIZONTAL)
			.Section(&elem.currentAnimation)
			.Scale({ 1.25f, 1.25f });

		app->render->DrawTexture(drawAlly);

		auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - elem.animTimer);
		if (timeElapsed.count() >= 150)
		{
			elem.currentAnimation.x += elem.currentAnimation.w;
			elem.animTimer = now;
			if (elem.currentAnimation.x >= elem.currentAnimation.w * 4)
			{
				elem.currentAnimation.x = 0;
			}
		}
	}

	for (auto& elem : enemies->troop)
	{
		DrawParameters drawEnemy(elem.battlerTextureID, elem.position);
		drawEnemy.Flip(SDL_FLIP_HORIZONTAL);
		drawEnemy.Scale(fPoint(2, 2));

		// Rotate texture if enemy is dead
		if (elem.IsDead())
		{
			drawEnemy.RotationAngle(270);

			drawEnemy.Center(SDL_Point(elem.size.x / 2, elem.size.y));
		}
		// If it's alive and we are hovering it
		else if (elem.IsMouseHovering() && (actionSelected == ActionNames::ATTACK || actionSelected == ActionNames::SPECIAL_ATTACK))
		{
			// Set the alpha value for the fade-in / fade-out
			SDL_SetTextureAlphaMod(app->GetTexture(elem.battlerTextureID), elem.alpha);

			elem.alpha += (5 * elem.fadingDirection);
			if (elem.alpha >= 255 || elem.alpha <= 0)
			{
				elem.fadingDirection *= -1;
			}
		}
		else
		{
			elem.alpha = 255;
		}

		app->render->DrawTexture(drawEnemy);

		elem.UpdateParticles();

		SDL_SetTextureAlphaMod(app->GetTexture(elem.battlerTextureID), 255);
	}

	if (!messages.IsActive() && state == BattleState::PLAYER_INPUT)
	{
		menu->Draw();
		if (menu->GetInStatsMenu())
		{
			return;
		}
	}
}

TransitionScene Scene_Battle::Update()
{
	switch (state)
	{
		using enum BattleState;
	case PLAYER_INPUT:
	{
		UpdatePlayerTurn();
		break;
	}
	case ENEMY_INPUT:
	{
		ChooseEnemyActions();
		break;
	}
	case RESOLUTION:
	{
		ResolveActionQueue();
		break;
	}
	case BATTLE_WON:
	{
		if(!bBattleResolved)
			ResolveWinningBattle();

		if (IsAdvanceTextButtonDown() && !messages.RemoveCurrentMessage())
		{
			return TransitionScene::WIN_BATTLE;
		}

		break;
	}
	case BATTLE_LOSS:
	{
		if (IsAdvanceTextButtonDown())
		{
			return TransitionScene::LOSE_BATTLE;
		}
		break;
	}
	case RUN_FROM_BATTLE:
	{
		if (IsAdvanceTextButtonDown())
		{
			return TransitionScene::RUN_BATTLE;
		}
		break;
	}
	}

	messages.UpdateAnimations();

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

void Scene_Battle::UpdatePlayerTurn()
{
	auto const& [success, action] = menu->Update();

	if (success)
	{
		if (menu->GetCompletedStatus())
		{
			std::deque<BattleAction> provisionalQueue = menu->GetActionQueue();
			while (!provisionalQueue.empty())
			{
				actionQueue.emplace(provisionalQueue.front());
				provisionalQueue.pop_front();
			}

			state = BattleState::ENEMY_INPUT;

			messages.RemoveCurrentMessage();

			return;
		}
		currentPlayer++;
		PlayBattlerSFX(party->party[currentPlayer]);
	}
	else
	{
		currentPlayer = action.source;
		actionSelected = action.action;

		UpdatePlayerInputText();
	}
}

void Scene_Battle::UpdatePlayerInputText()
{
	std::string text = "";

	for (auto& elem : enemies->troop)
	{
		if (elem.IsMouseHovering())
		{
			// Update text widget to show enemy name and stats
			text = std::format(
				"{} | HP: {} | Def: {} | Sp. Def: {}",
				elem.name,
				elem.currentHP,
				elem.GetStat(BaseStats::DEFENSE),
				elem.GetStat(BaseStats::SPECIAL_DEFENSE)
			);
			break;
		}
	}

	if(text.empty())
	{
		if (actionSelected == ActionNames::ATTACK || actionSelected == ActionNames::SPECIAL_ATTACK)
		{
			text = "Choose a target.";
		}
		else if (currentPlayer == -1)
		{
			text = "Fight or flee?";
		}
		else
		{
			text = std::format("What will {} do?", party->party[currentPlayer].name);
		}
	}

	messages.ReplaceCurrentMessage(text);
}

bool Scene_Battle::IsAdvanceTextButtonDown() const
{
	return (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN);
}

void Scene_Battle::ChooseEnemyActions()
{
	std::mt19937 gen(rd());
	random.param(std::uniform_int_distribution<>::param_type(0, party->party.size() - 1));

	for (int i = 0; auto const& elem : enemies->troop)
	{
		if (elem.IsDead())
		{
			i++;
			continue;
		}

		auto actionSpeed = elem.GetStat(BaseStats::SPEED);

		int target = 0;

		do
		{
			target = random(gen);
		} while (party->party[target].IsDead());


		if (int action = random100(gen);
			action <= 40)		// 40% chance
		{
			actionQueue.emplace(ActionNames::ATTACK, i, target, false, true, actionSpeed);
		}
		else if (action <= 80)	// 40% chance
		{
			actionQueue.emplace(ActionNames::SPECIAL_ATTACK, i, target, false, true, actionSpeed);
		}
		else					//20% chance
		{
			actionQueue.emplace(ActionNames::DEFEND, i, target, false, false, INT_MAX - 1);
		}

		i++;
	}

	messages.SetActive(true);
	state = BattleState::RESOLUTION;
}

void Scene_Battle::ResolveActionQueue()
{
	if (messages.IsInputLocked())
	{
		if(IsAdvanceTextButtonDown())
		{
			messages.RemoveCurrentMessage();

			if (checkBattleEnd)
			{
				CheckIfBattleWinThenChangeState();
				CheckBattleLossThenChangeState();

				checkBattleEnd = false;
			}
		}
	}
	else
	{
		while (!actionQueue.empty())
		{
			BattleAction currentAction = actionQueue.top();

			std::string resolveText = ResolveAction(currentAction);

			actionQueue.pop();

			if (!resolveText.empty())
			{
				messages.AddMessageToQueue(resolveText);
				messages.LockInput();
				return;
			}

		}

		currentPlayer = 0;
		menu->StartNewTurn();
		messages.SetActive(false);
		state = BattleState::PLAYER_INPUT;
	}
}

std::string Scene_Battle::ResolveAction(BattleAction const& currentAction)
{
	std::string text = "";

	if (currentAction.action == ActionNames::RUN || currentAction.source == -1)
	{
		text = "You run from battle. Shame.";
		PlayActionSFX("Run");
		state = BattleState::RUN_FROM_BATTLE;

		return text;
	}
	Battler& source = currentAction.friendlySource ? party->party[currentAction.source] : enemies->troop[currentAction.source];
	std::vector<Battler>& receiver = currentAction.friendlyTarget ? party->party : enemies->troop;

	ActionNames action = currentAction.action;


	if (source.IsDead())
	{
		return text;
	}

	if (currentAction.actionScope != Item::GeneralProperties::Scope::ONE_DEAD_ALLY && receiver[currentAction.target].IsDead())
	{
		action = ActionNames::DEFEND;
	}

	switch (action)
	{
		using enum ActionNames;
		case ATTACK:
		{
			text = BattlerAttacking(
				source,
				receiver[currentAction.target],
				BaseStats::ATTACK,
				BaseStats::DEFENSE
			);
			break;
		}
		case SPECIAL_ATTACK:
		{
			if (source.currentMana >= 4) 
			{
				text = BattlerAttacking(
					source,
					receiver[currentAction.target],
					BaseStats::SPECIAL_ATTACK,
					BaseStats::SPECIAL_DEFENSE
				);
				source.currentMana -= 4;
			}
			else 
			{
				text = std::format("{} does not have enough Mana!", source.name);
			}

			break;
		}
		case DEFEND:
		{
			text = BattlerDefending(source);
			break;
		}
		case RUN:
		{
			text = "You run from battle. Shame.";
			PlayActionSFX("Run");
			state = BattleState::RUN_FROM_BATTLE;

			break;
		}
		case ITEM:
		{
			Item const& itemUsed = party->dbItems->GetItem(currentAction.actionID);
			switch (currentAction.actionScope)
			{
				using enum Item::GeneralProperties::Scope;
			case ONE_DEAD_ALLY:
				receiver[currentAction.target].currentAnimation.y = 0;
			case ONE_ENEMY:
			case ONE_ALLY:
			{
				if (receiver[currentAction.target].UseItem(itemUsed))
				{
					party->RemoveItemFromInventory(currentAction.actionID);
				}
				messages.AddMessageToQueue(std::format("{} uses {} {}!", source.name, itemUsed.general.article, itemUsed.general.name));

				text = receiver[currentAction.target].GetTextToDisplay();
				break;
			}
			default:
				break;
			}
			break;
		}
		case NONE:
		{
			state = BattleState::BATTLE_LOSS;
			break;
		}
	}

	return text;
}

std::string Scene_Battle::BattlerDefending(Battler& battler) const
{
	battler.isDefending = true;
	PlayActionSFX("Block");

	return std::format("{} is defending.", battler.name);
}

std::string Scene_Battle::BattlerAttacking(Battler const& source, Battler& receiver, BaseStats offensiveStat, BaseStats defensiveStat)
{
	std::mt19937 gen(rd());

	int attack = source.GetStat(offensiveStat);
	int defense = receiver.GetStat(defensiveStat);

	int damage = 1;

	std::string damageMessage = "";

	if (random100(gen) <= 10)
	{
		damage = CalculateDamage(attack, defense, receiver.isDefending, 1.5f);
		damageMessage = "{} attacks {}! Criticals for {} damage!!!";
		PlayActionSFX("Critical");
	}
	else
	{
		damage = CalculateDamage(attack, defense, receiver.isDefending);
		damageMessage = "{} attacks {}! Deals {} damage.";
		PlayActionSFX("Attack");
	}

	receiver.isDefending = false;
	receiver.currentHP -= damage;

	if (receiver.currentHP <= 0)
	{
		BattlerJustDied(receiver);
		receiver.currentAnimation.y = 96;
	}

	return AddSaveData(damageMessage, source.name, receiver.name, damage);

}

int Scene_Battle::CalculateDamage(int atk, int def, bool defending, float crit)
{
	std::mt19937 gen(rd());

	int randomVariance = random40(gen);

	if (defending)
		def *= 2;

	int damage = atk - def;

	if (randomVariance > 20)
	{
		randomVariance -= 20;
		damage += static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);
	}
	else
		damage -= static_cast<int>(static_cast<float>(damage) * static_cast<float>(randomVariance) / 100.0f);

	damage = static_cast<int>(static_cast<float>(damage) * crit);

	return damage <= 0 ? 1 : damage;
}

void Scene_Battle::BattlerJustDied(Battler const& battler)
{
	app->audio->PlayFx(battler.deadSfx);
	checkBattleEnd = true;
}

void Scene_Battle::CheckIfBattleWinThenChangeState()
{
	if (std::ranges::none_of(enemies->troop, [](Battler const& e) { return e.currentHP > 0; }))
	{
		LOG("Battle won.");

		messages.ReplaceCurrentMessage("You won the battle!");
		state = BattleState::BATTLE_WON;
		app->audio->PlayMusic("Music/M_Battle-Win.ogg");
	}
}

void Scene_Battle::ResolveWinningBattle()
{
	std::vector<std::pair<std::string_view, int>> enemiesDefeated;
	int xpWon = 0;

	for (auto const& enemy : enemies->troop)
	{
		std::string_view enemyName = enemy.name;

		xpWon += enemy.GetStat(BaseStats::XP);

		bool enemyFound = false;
		for (auto& [name, amount] : enemiesDefeated)
		{
			if (name == enemyName)
			{
				amount++;
				enemyFound = true;
			}
		}
		if (!enemyFound)
		{
			enemiesDefeated.emplace_back(enemyName, 1);
		}
	}

	if (xpWon > 0)
	{
		messages.AddMessageToQueue(std::format("Party receives {} experience.", xpWon));
		for (auto& character : party->party)
		{
			if (!character.IsDead())
			{
				if (int newLevel = character.AddXP(xpWon);
					newLevel != -1)
				{
					messages.AddMessageToQueue(std::format("{} is now level {}!", character.name, newLevel));
				}
			}
		}
	}

	party->PossibleQuestProgress(QuestType::HUNT, enemiesDefeated, std::vector<std::pair<int, int>>());

	bBattleResolved = true;
}

void Scene_Battle::CheckBattleLossThenChangeState()
{
	if (bool result = std::ranges::any_of(party->party, [](Battler const& PC) { return PC.currentHP > 0; });
		!result)
	{
		LOG("Battle loss.");
		messages.ReplaceCurrentMessage("You lost the battle...");
		state = BattleState::BATTLE_LOSS;
	}
}

void Scene_Battle::PlayBattlerSFX(Battler const& battler) const
{
	PlayActionSFX(battler.name);
}

void Scene_Battle::PlayActionSFX(std::string_view sfxKey) const
{
	if (auto it = sfx.find(sfxKey);
		it != sfx.end())
	{
		app->audio->PlayFx(it->second);
	}
}

void Scene_Battle::DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const
{
	iPoint texSize = app->tex->GetSize(textureID);
	
	SDL_Rect hpBar{};
	hpBar.x = position.x + 2;
	hpBar.y = position.y + texSize.y * 2 + 10;
	hpBar.h = 10;

	// Draw Background bar with a fixed width of 100
	hpBar.w = 100;
	app->render->DrawShape(hpBar, true, SDL_Color(0, 0, 0, 255));

	// Draw HP bar with width equal to the % remaining
	float hp = static_cast<float>(currentHP) / static_cast<float>(maxHP);
	hpBar.w = hp > 0 ? static_cast<int>(hp * 100.0f) : 0;

	// Assign colour (Green = max HP -> Red = 0 HP) depending on % hp remaining
	auto red = static_cast<Uint8>(250.0f - (250.0f * hp));
	auto green = static_cast<Uint8>(250.0f * hp);

	app->render->DrawShape(hpBar, true, SDL_Color(red, green, 0, 255));
}

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

	CheckBattleLossThenChangeState();
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

	messages->ModifyLastWidgetText("");

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
}

void Scene_Battle::Start()
{
}

void Scene_Battle::Draw()
{
	app->fonts->SetScale(0, 1.5);
	SDL_Rect camera = app->render->GetCamera();
	app->fonts->DrawText(
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque eu metus in est gravida eleifend ultricies ac nulla.Vivamus scelerisque a mauris nec egestas.In hac habitasse platea dictumst.Donec sed nisi ac ex cursus vestibulum.Phasellus rutrum erat congue ex porttitor, nec elementum libero fringilla.Nam ultricies lacus id orci vestibulum elementum. Vivamus sodales eu turpis in ultrices.Duis tincidunt ex ligula, sed faucibus lectus placerat id. Nulla in metus nulla.Praesent fermentum urna id ligula pulvinar finibus.Mauris id rhoncus tortor, non lacinia diam.Aliquam semper, urna tincidunt tempus accumsan, dui nibh condimentum tortor, vitae facilisis quam velit sit amet metus.Nullam dapibus quam sed nibh fringilla pretium.Cras consequat, metus sed facilisis ultrices, sem dui mattis nunc, faucibus euismod justo orci ut sem.Donec aliquet quam quis neque venenatis, sit amet ornare mi rutrum.Curabitur auctor leo vel ullamcorper facilisis.Integer turpis diam, aliquet vel egestas ac, facilisis sed risus.Maecenas nec facilisis massa.Vivamus pulvinar non metus vitae volutpat.Proin arcu nisi, faucibus lacinia fringilla non, facilisis non ipsum.Proin risus odio, pharetra eget bibendum sed, luctus vel purus.Cras efficitur orci nec urna imperdiet, nec blandit purus porttitor.Nam augue erat, posuere quis rutrum ac, porta et sem.Vestibulum eget venenatis ex. Nam risus nibh, auctor auctor mollis sit amet, scelerisque et quam.Pellentesque est erat, luctus vel magna non, fermentum aliquam leo.Etiam vestibulum dui mi, eget elementum lacus malesuada ornare.Nulla nec laoreet massa.Phasellus aliquam congue erat porttitor tristique.Nam laoreet aliquam tempor.Vivamus tincidunt purus nec molestie ullamcorper.Cras a vulputate urna.Integer elementum aliquet viverra.Pellentesque aliquet neque sagittis sem feugiat, ut viverra lectus euismod.Donec eleifend sapien ante, ac pharetra leo varius eget.Vestibulum leo felis, lobortis sit amet condimentum a, tincidunt vel ipsum.Curabitur arcu magna, sagittis vel feugiat vel, congue id odio.Donec augue felis, aliquet vel tempus vel, laoreet non neque. Aliquam consequat lorem sed consequat cursus.Cras ut condimentum justo, vitae scelerisque tortor.",
		true,
		TextParameters(0, DrawParameters(0, iPoint(0, 0)).Section(&camera))
	);
	app->fonts->SetScale(0, 2);

	//app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(-camera.x, -camera.y)));
	//
	//for (auto const& elem : windows)
	//{
	//	elem->Draw();
	//}

	//messages->Draw();
	//actions->Draw();

	//std::string text = "";

	//for (int i = 0; auto const& elem : party->party)
	//{
	//	iPoint allyPosition(300 - camera.x, (120 * i) - camera.y + 55);
	//	iPoint hpBarPosition(270 - camera.x, (120 * i) - camera.y + 80);

	//	DrawHPBar(
	//		elem.battlerTextureID,
	//		elem.currentHP,
	//		elem.GetStat(BaseStats::MAX_HP),
	//		hpBarPosition
	//	);

	//	DrawParameters drawAlly(elem.battlerTextureID, allyPosition);

	//	if (elem.IsDead())
	//	{
	//		drawAlly.RotationAngle(90);

	//		SDL_Point center = app->tex->GetSizeSDLPoint(elem.battlerTextureID);
	//		center.x /= 2;

	//		drawAlly.Center(center);
	//	}

	//	drawAlly.Scale(fPoint(3, 3));

	//	app->render->DrawTexture(drawAlly);

	//	i++;
	//}

	//for (auto& elem : enemies.troop)
	//{
	//	DrawHPBar(
	//		elem.battlerTextureID,
	//		elem.currentHP,
	//		elem.GetStat(BaseStats::MAX_HP),
	//		elem.position
	//	);

	//	DrawParameters drawEnemy(elem.battlerTextureID, elem.position);
	//	drawEnemy.Flip(SDL_FLIP_HORIZONTAL);
	//	drawEnemy.Scale(fPoint(2, 2));

	//	// Rotate texture if enemy is dead
	//	if (elem.IsDead())
	//	{
	//		drawEnemy.RotationAngle(270);

	//		drawEnemy.Center(SDL_Point(elem.size.x / 2, elem.size.y));
	//	}
	//	// If it's alive and we are hovering it
	//	else if (elem.IsMouseHovering() && (actionSelected == ActionNames::ATTACK || actionSelected == ActionNames::SPECIAL_ATTACK))
	//	{
	//		// Set the alpha value for the fade-in / fade-out
	//		SDL_SetTextureAlphaMod(app->GetTexture(elem.battlerTextureID), elem.alpha);

	//		elem.alpha += (5 * elem.fadingDirection);
	//		if (elem.alpha >= 255 || elem.alpha <= 0)
	//		{
	//			elem.fadingDirection *= -1;
	//		}
	//	}
	//	else
	//	{
	//		elem.alpha = 255;
	//	}

	//	app->render->DrawTexture(drawEnemy);

	//	SDL_SetTextureAlphaMod(app->GetTexture(elem.battlerTextureID), 255);
	//}
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

		if (IsAdvanceTextButtonDown())
		{
			if (messageQueue.empty())
				return TransitionScene::WIN_BATTLE;
			
			messages->ModifyLastWidgetText(messageQueue.front());
			messageQueue.pop();
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
	int prevBattler = currentPlayer;

	if (currentPlayer < party->party.size()
		&& (party->party[currentPlayer].IsDead() || ResolveMouseClick() || CharacterChooseAction()))
	{
		currentPlayer++;
	}

	if (prevBattler != currentPlayer)
	{
		ToggleRunButton();

		if (currentPlayer >= party->party.size())
		{
			state = BattleState::ENEMY_INPUT;
			return;
		}

		PlayBattlerSFX(party->party[currentPlayer]);
	}
}

void Scene_Battle::ToggleRunButton()
{
	if (currentPlayer != 0)
		actions->widgets.back()->Disable();
	else
		actions->widgets.back()->Enable();
}

bool Scene_Battle::ResolveMouseClick()
{
	using enum Scene_Battle::ActionNames;
	if (actionSelected == ATTACK || actionSelected == SPECIAL_ATTACK)
	{
		if (ChooseTarget())
		{
			auto actionSpeed = party->party[currentPlayer].GetStat(BaseStats::SPEED);

			actionQueue.emplace(actionSelected, currentPlayer, targetSelected, true, actionSpeed);

			actionSelected = NONE;
			targetSelected = -1;

			dynamic_cast<GuiButton*>(actions->widgets[actions->lastWidgetInteractedIndex].get())->ToggleSelected();

			return true;
		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
		{
			actionSelected = NONE;
			targetSelected = -1;

			dynamic_cast<GuiButton*>(actions->widgets[actions->lastWidgetInteractedIndex].get())->ToggleSelected();
		}
	}

	return false;
}

bool Scene_Battle::ChooseTarget()
{
	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
	{
		for (auto const& elem : enemies.troop)
		{
			if (elem.IsMouseHovering() && !elem.IsDead())
			{
				targetSelected = elem.index;
				return true;
			}
		}
	}

	return false;
}

bool Scene_Battle::CharacterChooseAction()
{
	std::string text = "";

	for (auto& elem : enemies.troop)
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
		else
		{
			text = std::format("What will {} do?", party->party[currentPlayer].name);
		}
	}

	messages->ModifyLastWidgetText(text);

	switch (actions->Update())
	{
		case 100:
		{
			actionSelected = ActionNames::ATTACK;
			dynamic_cast<GuiButton*>(actions->widgets[actions->lastWidgetInteractedIndex].get())->ToggleSelected();
			break;
		}
		case 101:
		{
			actionSelected = ActionNames::SPECIAL_ATTACK;
			dynamic_cast<GuiButton*>(actions->widgets[actions->lastWidgetInteractedIndex].get())->ToggleSelected();
			break;
		}
		case 102:
		{
			actionQueue.emplace(ActionNames::DEFEND, currentPlayer, 0, true, INT_MAX);

			return true;
		}
		case 103:
		{
			if (currentPlayer == 0)
			{
				actionQueue.emplace(ActionNames::RUN, currentPlayer, 0, true, INT_MAX);
				currentPlayer = party->party.size();
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return false;
}

bool Scene_Battle::IsAdvanceTextButtonDown() const
{
	return (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN);
}

void Scene_Battle::ChooseEnemyActions()
{
	std::mt19937 gen(rd());
	random.param(std::uniform_int_distribution<>::param_type(0, party->party.size() - 1));

	for (int i = 0; auto const& elem : enemies.troop)
	{
		auto actionSpeed = elem.GetStat(BaseStats::SPEED);

		int target = 0;

		do
		{
			target = random(gen);
		} while (party->party[target].IsDead());


		if (int action = random100(gen);
			action <= 40)		// 40% chance
		{
			actionQueue.emplace(ActionNames::ATTACK, i, target, false, actionSpeed);
		}
		else if (action <= 80)	// 40% chance
		{
			actionQueue.emplace(ActionNames::SPECIAL_ATTACK, i, target, false, actionSpeed);
		}
		else					//20% chance
		{
			actionQueue.emplace(ActionNames::DEFEND, i, target, false, INT_MAX);
		}

		i++;
	}

	state = BattleState::RESOLUTION;
}

void Scene_Battle::ResolveActionQueue()
{
	if (currentlyInTextMessage && IsAdvanceTextButtonDown())
	{
		if (checkBattleEnd)
		{
			CheckIfBattleWinThenChangeState();
			CheckBattleLossThenChangeState();

			checkBattleEnd = false;

			if (state != BattleState::RESOLUTION)
				return;
		}

		currentlyInTextMessage = false;
	}

	if (!currentlyInTextMessage)
	{
		if (actionQueue.empty())
		{
			messages->ModifyLastWidgetText("");
			currentPlayer = 0;
			ToggleRunButton();
			state = BattleState::PLAYER_INPUT;

			return;
		}

		BattleAction currentAction = actionQueue.top();

		if (std::string text = ResolveAction(currentAction);
			!text.empty())
		{
			messages->ModifyLastWidgetText(text);
			currentlyInTextMessage = true;
		}

		actionQueue.pop();
	}
}

std::string Scene_Battle::ResolveAction(BattleAction const& currentAction)
{
	std::string text = "";

	Battler& source = currentAction.friendly ? party->party[currentAction.source] : enemies.troop[currentAction.source];
	std::vector<Battler>& receiver = currentAction.friendly ? enemies.troop :  party->party;

	ActionNames action = currentAction.action;

	if (source.IsDead())
	{
		return text;
	}

	if (receiver[currentAction.target].IsDead())
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
			text = BattlerAttacking(
				source,
				receiver[currentAction.target],
				BaseStats::SPECIAL_ATTACK,
				BaseStats::SPECIAL_DEFENSE
			);
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
		case USE_ITEM:
		{
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
	if (std::ranges::none_of(enemies.troop, [](Battler const& e) { return e.currentHP > 0; }))
	{
		LOG("Battle won.");

		messages->ModifyLastWidgetText("You won the battle!");
		state = BattleState::BATTLE_WON;
		app->audio->PlayMusic("Music/M_Battle-Win.ogg");
	}
}

void Scene_Battle::ResolveWinningBattle()
{
	std::vector<std::pair<std::string_view, int>> enemiesDefeated;
	int xpWon = 0;

	for (auto const& enemy : enemies.troop)
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
		messageQueue.emplace(std::format("Party receives {} experience.", xpWon));
		for (auto& character : party->party)
		{
			if (!character.IsDead())
			{
				if (int newLevel = character.AddXP(xpWon);
					newLevel != -1)
				{
					messageQueue.emplace(std::format("{} is now level {}!", character.name, newLevel));
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
		messages->ModifyLastWidgetText("You lost the battle...");
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

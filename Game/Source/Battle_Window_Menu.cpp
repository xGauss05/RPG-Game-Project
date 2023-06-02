#include "App.h"
#include "Render.h"
#include "Input.h"

#include "Battle_Window_Menu.h"

#include "Log.h"

Battle_Window_Menu::Battle_Window_Menu(Window_Factory const& windowFac)
	: statsWindow(windowFac.CreateWindow("StatsMenu"))
{
	using enum Battle_Window_Menu::MenuWindows;
	menuLogic.AddVertex(MAIN);
	panels.emplace_back(windowFac.CreateMenuList("MenuBattleFightOrFlee"));
	panels.back()->SetActive(true);
	menuLogic.AddVertex(FIGHT);
	panels.emplace_back(windowFac.CreateMenuList("MenuBattleActions"));
	panels.back()->SetActive(false);
	mainMenuTexture = dynamic_cast<Battle_Menu_Main*>(panels.back().get())->GetTextureID();

	menuLogic.AddVertex(INVENTORY);
	panels.emplace_back(windowFac.CreateMenuList("BattleInventory"));
	panels.back()->SetActive(false);

	menuLogic.AddVertex(FLEE);
	menuLogic.AddVertex(STATS);
	menuLogic.AddVertex(ATTACK);
	menuLogic.AddVertex(SPECIAL);
	menuLogic.AddVertex(DEFEND);
	menuLogic.AddVertex(SELECT_TARGET);

	cursor.textureID = app->tex->Load("Assets/UI/Cursor.png");
	cursor.srcRect =
	{
		0,
		0,
		app->tex->GetSize(cursor.textureID).x,
		app->tex->GetSize(cursor.textureID).y
	};
}

void Battle_Window_Menu::InitializeLogicGraph()
{
	using enum Battle_Window_Menu::MenuWindows;
	menuLogic.AddEdge(MAIN, FIGHT);
	menuLogic.AddEdge(MAIN, FLEE);
	menuLogic.AddEdge(MAIN, STATS);
	menuLogic.AddEdge(FIGHT, ATTACK);
	menuLogic.AddEdge(ATTACK, SELECT_TARGET);
	menuLogic.AddEdge(FIGHT, SPECIAL);
	menuLogic.AddEdge(SPECIAL, SELECT_TARGET);
	menuLogic.AddEdge(FIGHT, INVENTORY);
	menuLogic.AddEdge(INVENTORY, SELECT_TARGET);
	menuLogic.AddEdge(FIGHT, DEFEND);
}

void Battle_Window_Menu::Start()
{
	for (auto const &elem : panels)
	{
		elem->Initialize();
	}

	InitializeLogicGraph();

	panels[currentActivePanel]->Start();

	enabled = true;
}

std::pair<bool, BattleAction>  Battle_Window_Menu::Update()
{
	if (bInputCompleted)
	{
		while (!panelHistory.empty() && menuLogic.At(currentActivePanel).value != MenuWindows::FIGHT)
		{
			currentActivePanel = panelHistory.top();
			panelHistory.pop();
		}
		panels[currentActivePanel]->SetActive(true);
		panels[currentActivePanel]->Start();

		return { true, actionQueue.back() };
	}

	if (cursor.enabled)
	{
		if(app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			for (auto const& elem : *currentTargetParty)
			{
				if (elem.IsMouseHovering() && !elem.IsDead())
				{
					currentAction.target = elem.index;
					actionQueue.push_back(currentAction);
					cursor.enabled = false;
					currentAction.ResetAction();

					currentSource = GetNextBattler();
					currentAction.source = currentSource;

					if (currentSource == -1)
					{
						bInputCompleted = true;
					}
					else
					{
						while (menuLogic.At(currentActivePanel).value != MenuWindows::FIGHT)
						{
							currentActivePanel = panelHistory.top();
							panelHistory.pop();
						}

						panels[currentActivePanel]->SetActive(true);
						panels[currentActivePanel]->Start();
					}

					return { true, actionQueue.back() };
				}
			}
		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
		{
			cursor.enabled = false;
		}

		SDL_Rect camera = app->render->GetCamera();
		iPoint drawPosition = { camera.x * -1, camera.y * -1 };

		drawPosition =
		{
			drawPosition.x + (camera.w / 2) - (app->tex->GetSize(mainMenuTexture).y * 3) + 8,
			drawPosition.y + camera.h - app->tex->GetSize(mainMenuTexture).y
		};

		app->render->DrawTexture(
			DrawParameters(
				cursor.battlerTexture,
				drawPosition
			)
		);

		drawPosition.x += app->tex->GetSize(mainMenuTexture).y;

		SDL_Rect dstRect =
		{
			drawPosition.x + camera.x,
			drawPosition.y + camera.y,
			app->tex->GetSize(mainMenuTexture).x - 16,
			app->tex->GetSize(mainMenuTexture).y
		};

		drawPosition.x += (app->tex->GetSize(mainMenuTexture).y * 4);

		app->render->DrawTexture(
			DrawParameters(
				mainMenuTexture,
				drawPosition
			).Section(&cursor.actionSection)
		);

		cursor.actionMessage.SetPanelArea(dstRect);

		cursor.actionMessage.Draw();

		iPoint textPosition =
		{
			(camera.w / 2),
			camera.h - (app->tex->GetSize(mainMenuTexture).y / 2)
		};

		TextParameters aux =
		{
			0,
			{ 0, textPosition}
		};

		app->fonts->DrawText(cursor.actionName, TextParameters(0, { 0, textPosition }).Align(AlignTo::ALIGN_CENTER));

		return { false, currentAction };
	}

	if (bInStatsMenu)
	{
		statsWindow->Update();

		if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
		{
			bInStatsMenu = false;
		}
	}
	else if(!panels.empty() && panels[currentActivePanel]->Update())
	{
		if (panels[currentActivePanel]->GoToPreviousMenu())
		{
			GoToPreviousPanel();
		}
		else
		{
			GoToNextPanel();
		}
	}

	return { false, currentAction };
}

void Battle_Window_Menu::Draw() const
{
	if (bInStatsMenu)
	{
		DrawStatsMenu();
		return;
	}

	if (cursor.enabled)
	{
		for (auto const& elem : *currentTargetParty)
		{
			if (elem.IsMouseHovering() && !elem.IsDead())
			{
				iPoint drawPosition = elem.position;
				drawPosition.x += (elem.size.x / 2);
				app->render->DrawTexture(DrawParameters(cursor.textureID, drawPosition).Section(&cursor.srcRect));
			}
		}


	}
	else
	{
		for (auto const& elem : panels)
		{
			if (elem->IsActive())
				elem->Draw();
		}

		if (currentSource != -1)
		{
			SDL_Rect camera = app->render->GetCamera();
			iPoint drawPosition = { camera.x * -1, camera.y * -1 };

			drawPosition =
			{
				drawPosition.x + (camera.w / 2) - (app->tex->GetSize(mainMenuTexture).y * 3) + 8,
				drawPosition.y + camera.h - app->tex->GetSize(mainMenuTexture).y
			};

			app->render->DrawTexture(
				DrawParameters(
					playerParty->party[currentSource].portraitTextureID,
					drawPosition
				)
			);
		}
	}
}

void Battle_Window_Menu::SetPlayerParty(GameParty* party)
{
	playerParty = party;

	for (int i = 0; i < menuLogic.GetGraphSize(); ++i)
	{
		switch (menuLogic.At(i).value)
		{
			using enum Battle_Window_Menu::MenuWindows;
			case FIGHT:
			case INVENTORY:
			{
				dynamic_cast<Map_Menu_ComponentParty*>(panels[i].get())->SetGameParty(party);
				break;
			}
			default:
			{
				continue;
			}
		}
	}
}

void Battle_Window_Menu::SetEnemyTroop(EnemyTroops* enemytroop)
{
	enemies = enemytroop;
}

bool Battle_Window_Menu::GetCompletedStatus() const
{
	return bInputCompleted;
}

bool Battle_Window_Menu::GetInStatsMenu() const
{
	return bInStatsMenu;
}

void Battle_Window_Menu::StartNewTurn()
{
	currentAction.ResetAction();

	cursor.enabled = false;
	
	while (!panelHistory.empty())
	{
		panels[currentActivePanel]->SetActive(false);
		currentActivePanel = panelHistory.top();
		panelHistory.pop();
	}

	currentActivePanel = 0;

	panels[currentActivePanel]->SetActive(true);
	panels[currentActivePanel]->Start();
	
	for (auto const& elem : playerParty->party)
	{
		if (!elem.IsDead())
		{
			currentSource = elem.index;
			break;
		}
	}

	currentTargetParty = nullptr;

	actionQueue.clear();

	bInputCompleted = false;
	bInStatsMenu = false;
}

void Battle_Window_Menu::GoToNextPanel()
{
	if (menuLogic.At(currentActivePanel).edges.empty())
		return;

	int buttonClicked = panels[currentActivePanel]->GetLastClick();
	int nextPanelID = buttonClicked < menuLogic.At(currentActivePanel).edges.size() ? 
		menuLogic.At(currentActivePanel).edges[buttonClicked].destination :
		menuLogic.At(currentActivePanel).edges[0].destination;

	if (MenuWindows nextPanel = menuLogic.At(nextPanelID).value;
		nextPanel == MenuWindows::STATS)
	{
		bInStatsMenu = true;
	}
	else if (nextPanel == MenuWindows::ATTACK || nextPanel == MenuWindows::SPECIAL)
	{
		auto actionSpeed = playerParty->party[currentSource].GetStat(BaseStats::SPEED);

		currentAction.action = nextPanel == MenuWindows::ATTACK ? ActionNames::ATTACK : ActionNames::SPECIAL_ATTACK;
		currentAction.friendlySource = true;
		currentAction.friendlyTarget = false;
		currentAction.source = currentSource;
		currentAction.speed = actionSpeed;
		
		SDL_Point textureSize = app->tex->GetSizeSDLPoint(mainMenuTexture);

		cursor.battlerTexture = playerParty->party[currentSource].portraitTextureID;
		cursor.actionSection =
		{
			textureSize.y * panels[currentActivePanel]->GetLastClick(),
			0,
			textureSize.y,
			textureSize.y
		};
		cursor.actionName = nextPanel == MenuWindows::ATTACK ? "Attack" : "Special";

		currentTargetParty = &enemies->troop;
		cursor.enabled = true;
	}
	else if (menuLogic.At(currentActivePanel).value == MenuWindows::INVENTORY)
	{
		panels[currentActivePanel]->SetActive(false);
		currentAction.action = ActionNames::ITEM;
		currentAction.friendlySource = true;
		currentAction.source = currentSource;
		currentAction.actionID = dynamic_cast<Battle_Menu_Inventory*>(panels[currentActivePanel].get())->GetUsedItemID();
		const Item& item = playerParty->dbItems->GetItem(currentAction.actionID);
		currentAction.actionScope = item.general.scope;
		currentAction.speed = item.invocation.speed + playerParty->party[currentSource].GetStat(BaseStats::SPEED);

		switch (currentAction.actionScope)
		{
			using enum Item::GeneralProperties::Scope;
			case ONE_ENEMY:
			case ALL_ENEMIES:
			case ONE_RANDOM_ENEMY:
			case TWO_RANDOM_ENEMIES:
			case THREE_RANDOM_ENEMIES:
				currentTargetParty = &enemies->troop;
				currentAction.friendlyTarget = false;
				break;
			case USER:
			case ONE_ALLY:
			case ALL_ALLIES:
			case ONE_DEAD_ALLY:
			case ALL_DEAD_ALLIES:
				currentTargetParty = &playerParty->party;
				currentAction.friendlyTarget = true;
				break;
			case NONE:
				LOG("SOMETHING WENT WRONG AND ACTION HAS NO SCOPE");
				currentTargetParty = &enemies->troop;
				currentAction.friendlyTarget = false;
				break;
		}

		cursor.enabled = true;
	}
	else if (nextPanel == MenuWindows::FLEE)
	{
		currentAction.action = ActionNames::RUN;
		currentAction.speed = INT_MAX;
		actionQueue.push_back(currentAction);

		currentAction.ResetAction();
		
		bInputCompleted = true;
	}
	else if (nextPanel == MenuWindows::DEFEND)
	{
		currentAction.action = ActionNames::DEFEND;
		currentAction.friendlySource = true;
		currentAction.friendlyTarget = true;
		currentAction.source = currentSource;
		currentAction.speed = INT_MAX - 1;
		currentAction.target = currentSource;

		actionQueue.push_back(currentAction);

		currentAction.ResetAction();

		currentSource = GetNextBattler();
		currentAction.source = currentSource;

		if (currentSource == -1)
		{
			bInputCompleted = true;
		}
	}
	else
	{
		if (currentAction.source == -1)
		{
			currentAction.source = currentSource;
		}
		panels[currentActivePanel]->SetActive(false);
		panelHistory.push(currentActivePanel);
		currentActivePanel = nextPanelID;
		panels[currentActivePanel]->Start();
		panels[currentActivePanel]->SetActive(true);
		bInStatsMenu = false;
	}
}

void Battle_Window_Menu::GoToPreviousPanel()
{
	if (panelHistory.empty())
	{
		panels[currentActivePanel]->Start();
		return;
	}

	if (menuLogic.At(currentActivePanel).value == MenuWindows::FIGHT && currentSource > 0)
	{
		currentSource--;
		actionQueue.pop_back();
		currentAction.ResetAction();
		currentAction.source = currentSource;
		panels[currentActivePanel]->Start();
		return;
	}

	panels[currentActivePanel]->SetActive(false);
	currentActivePanel = panelHistory.top();
	panels[currentActivePanel]->SetActive(true);
	panels[currentActivePanel]->Start();
	panelHistory.pop();
}

int Battle_Window_Menu::GetNextBattler() const
{
	int curr = currentSource + 1;
	while (curr < playerParty->party.size() && playerParty->party[curr].IsDead())
	{
		curr++;
	}

	if (curr == playerParty->party.size())
		return -1;

	return curr;
}

std::deque<BattleAction> &&Battle_Window_Menu::GetActionQueue()
{
	return std::move(actionQueue);
}

void Battle_Window_Menu::DrawStatsMenu() const
{
	statsWindow->Draw();

	std::ranges::for_each(playerParty->party,
		[this, positionX = 0](Battler const& c) mutable
		{
			DrawPlayerStats(c, positionX);
			positionX += 140;
		}
	);

}

void Battle_Window_Menu::DrawPlayerStats(Battler const& character, int i) const
{
	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };

	iPoint allyPosition(170 - camera.x, i - camera.y + 55);
	iPoint hpBarPosition(140 - camera.x, i - camera.y + 80);

	DrawHPBar(character.battlerTextureID, character.currentHP, character.GetStat(BaseStats::MAX_HP), hpBarPosition);

	DrawParameters drawAlly(character.battlerTextureID, allyPosition);

	if (character.currentHP <= 0)
	{
		drawAlly.RotationAngle(90);

		int w = 0;
		int h = 0;
		app->tex->GetSize(app->GetTexture(character.battlerTextureID), w, h);

		SDL_Point pivot = {
			w / 2,
			h
		};

		drawAlly.Center(pivot);
	}

	drawAlly.Scale(fPoint(3.0f, 3.0f));

	app->render->DrawTexture(drawAlly);

	using enum BaseStats;

	DrawSingleStat(character, MAX_HP, 280, 50 + i);
	DrawSingleStat(character, ATTACK, 620, 50 + i);
	DrawSingleStat(character, SPECIAL_ATTACK, 860, 50 + i);

	DrawSingleStat(character, MAX_MANA, 280, 90 + i);
	DrawSingleStat(character, DEFENSE, 620, 90 + i);
	DrawSingleStat(character, SPECIAL_DEFENSE, 860, 90 + i);

	DrawSingleStat(character, LEVEL, 450, 130 + i);
	DrawSingleStat(character, XP, 620, 130 + i);
	DrawSingleStat(character, SPEED, 860, 130 + i);

	app->fonts->DrawText(
		character.name,
		iPoint(280, 130 + i)
	);
}

void Battle_Window_Menu::DrawSingleStat(Battler const& character, BaseStats stat, int x, int y) const
{
	app->fonts->DrawText(
		character.GetStatDisplay(stat),
		iPoint(x, y)
	);
}

void Battle_Window_Menu::DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const
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

void BattleAction::ResetAction()
{
	action = ActionNames::NONE;
	source = -1;
	target = -1;
	friendlySource = false;
	friendlyTarget = false;
	speed = -1;
}

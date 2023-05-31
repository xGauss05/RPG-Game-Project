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

	menuLogic.AddVertex(FLEE);
	menuLogic.AddVertex(STATS);
	menuLogic.AddVertex(ATTACK);
	menuLogic.AddVertex(SPECIAL);
	menuLogic.AddVertex(INVENTORY);
	menuLogic.AddVertex(DEFEND);

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
	menuLogic.AddEdge(FIGHT, SPECIAL);
	menuLogic.AddEdge(FIGHT, INVENTORY);
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

					return { true, actionQueue.back() };
				}
			}
		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
		{
			cursor.enabled = false;
		}

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

	for (auto const& elem : panels)
	{
		if (elem->IsActive())
			elem->Draw();
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

Battle_Window_Menu::MenuWindows Battle_Window_Menu::GetCurrentState() const
{
	using enum Battle_Window_Menu::MenuWindows;
	if (cursor.enabled)		return BATTLE_CURSOR;
	else if (bInStatsMenu)	return STATS;
	else					return static_cast<Battle_Window_Menu::MenuWindows>(currentActivePanel);
}

bool Battle_Window_Menu::GetCompletedStatus() const
{
	return bInputCompleted;
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

	currentSource = 0;
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
	int nextPanelID = menuLogic.At(currentActivePanel).edges[buttonClicked].destination;

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

		currentTargetParty = &enemies->troop;
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

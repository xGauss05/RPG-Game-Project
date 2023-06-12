#include "App.h"
#include "Render.h"
#include "Input.h"

#include "Map_Window_Menu.h"

#include "Log.h"

Map_Window_Menu::Map_Window_Menu(Window_Factory const& windowFac)
	: statsWindow(windowFac.CreateWindow("StatsMenu"))
{
	using enum Map_Window_Menu::MenuWindows;
	menuLogic.AddVertex(MAIN);
	panels.emplace_back(windowFac.CreateMenuList("MenuCategories"));
	menuLogic.AddVertex(INVENTORY);
	panels.emplace_back(windowFac.CreateMenuList("MenuInventory"));
	menuLogic.AddVertex(COINS);
	panels.emplace_back(windowFac.CreateGoldDisplay());
	menuLogic.AddVertex(CHOOSE_CHARACTER);
	panels.emplace_back(windowFac.CreateMenuList("MenuCharacters"));

	menuLogic.AddVertex(STATS);
}

void Map_Window_Menu::InitializeLogicGraph()
{
	using enum Map_Window_Menu::MenuWindows;
	menuLogic.AddEdge(MAIN, INVENTORY);
	menuLogic.AddEdge(MAIN, STATS);
	menuLogic.AddEdge(INVENTORY, CHOOSE_CHARACTER);
}

void Map_Window_Menu::Start()
{
	for (auto const &elem : panels)
	{
		elem->Initialize();
	}

	InitializeLogicGraph();

	panels[currentActivePanel]->Start();

	enabled = true;
}

bool Map_Window_Menu::Update()
{
	if (bInStatsMenu)
	{
		statsWindow->Update();

		if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN
			|| app->input->GetKey(SDL_SCANCODE_X) == KeyState::KEY_DOWN) // Controller equivalent :/
		{
			bInStatsMenu = false;
		}

		return true;
	}

	if(!panels.empty() && panels[currentActivePanel]->Update())
	{
		if (panels[currentActivePanel]->GetAndDefaultCloseAllMenus())
		{
			return false;
		}

		if (panels[currentActivePanel]->GoToPreviousMenu())
		{
			if (panelHistory.empty())
				return false;
			
			GoToPreviousPanel();
		}
		else
		{
			GoToNextPanel();
		}
	}

	return true;
}

void Map_Window_Menu::Draw() const
{
	if (bInStatsMenu)
	{
		DrawStatsMenu();
	}
	else
	{
		for (auto const& elem : panels)
		{
			if(elem->IsActive())
				elem->Draw();
		}
	}
}

void Map_Window_Menu::SetPlayerParty(GameParty* party)
{
	playerParty = party;

	for (int i = 0; i < menuLogic.GetGraphSize(); ++i)
	{
		switch (menuLogic.At(i).value)
		{
			using enum Map_Window_Menu::MenuWindows;
			case CHOOSE_CHARACTER:
			case INVENTORY:
			case COINS:
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

void Map_Window_Menu::GoToNextPanel()
{
	if (menuLogic.At(currentActivePanel).edges.empty())
		return;

	int buttonClicked = panels[currentActivePanel]->GetLastClick();
	int nextPanelID = menuLogic.At(currentActivePanel).value == MenuWindows::INVENTORY ?
		menuLogic.At(currentActivePanel).edges[0].destination :
		menuLogic.At(currentActivePanel).edges[buttonClicked].destination;

	MenuWindows nextPanel = menuLogic.At(nextPanelID).value;

	if (nextPanel == MenuWindows::STATS)
	{
		bInStatsMenu = true;
	}
	else
	{
		if (nextPanel == MenuWindows::CHOOSE_CHARACTER)
		{
			panels[currentActivePanel]->SetActive(false);
			panels[nextPanelID]->SetActive(true);
		}
		panelHistory.push(currentActivePanel);
		currentActivePanel = nextPanelID;
		panels[currentActivePanel]->Start();
		bInStatsMenu = false;
	}
}

void Map_Window_Menu::GoToPreviousPanel()
{
	int lastPanelClick = panels[currentActivePanel]->GetLastClick();

	if (menuLogic.At(currentActivePanel).value == MenuWindows::CHOOSE_CHARACTER
		&& menuLogic.At(panelHistory.top()).value == MenuWindows::INVENTORY)
	{
		auto inventoryPanel = dynamic_cast<Map_Menu_Inventory*>(panels[panelHistory.top()].get());
		inventoryPanel->SetPreviousPanelLastClick(lastPanelClick);
		if (inventoryPanel->UseItem())
		{
			return;
		}
		panels[currentActivePanel]->SetActive(false);
	}

	currentActivePanel = panelHistory.top();
	panels[currentActivePanel]->SetActive(true);
	panelHistory.pop();
	panels[currentActivePanel]->SetPreviousPanelLastClick(lastPanelClick);
}

void Map_Window_Menu::DrawStatsMenu() const
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

void Map_Window_Menu::DrawPlayerStats(Battler const& character, int i) const
{
	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };

	iPoint allyPosition(140 - camera.x, i - camera.y + 30);

	DrawParameters drawAlly(character.portraitTextureID, allyPosition);

	if (character.currentHP <= 0)
	{
		drawAlly.RotationAngle(90);

		int w = 0;
		int h = 0;
		app->tex->GetSize(app->GetTexture(character.portraitTextureID), w, h);

		SDL_Point pivot = {
			w / 2,
			h
		};

		drawAlly.Center(pivot);
	}

	app->render->DrawTexture(drawAlly);

	using enum BaseStats;

	DrawSingleStat(character, MAX_HP, 280, 50 + i);
	DrawSingleStat(character, ATTACK, 620, 50 + i);
	DrawSingleStat(character, SPECIAL_ATTACK, 860, 50 + i);

	DrawSingleStat(character, MAX_MANA, 280, 94 + i);
	DrawSingleStat(character, DEFENSE, 620, 94 + i);
	DrawSingleStat(character, SPECIAL_DEFENSE, 860, 94 + i);

	DrawSingleStat(character, LEVEL, 450, 138 + i);
	DrawSingleStat(character, XP, 620, 138 + i);
	DrawSingleStat(character, SPEED, 860, 138 + i);

	app->fonts->DrawText(
		character.name,
		iPoint(280, 138 + i)
	);
}

void Map_Window_Menu::DrawSingleStat(Battler const& character, BaseStats stat, int x, int y) const
{
	app->fonts->DrawText(
		character.GetStatDisplay(stat),
		iPoint(x, y)
	);
}

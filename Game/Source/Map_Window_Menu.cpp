#include "App.h"
#include "Render.h"
#include "Input.h"

#include "Map_Window_Menu.h"

#include "Log.h"

Map_Window_Menu::Map_Window_Menu(Window_Factory const& windowFac)
	: statsWindow(windowFac.CreateWindow("StatsMenu"))
{
	using enum Map_Window_Menu::MenuWindows;
	panels.emplace_back(windowFac.CreateMenuList("Menu_MainCategories_NoIcons"));
	menuLogic.AddVertex(MAIN);
	panels.emplace_back(windowFac.CreateMenuList("Menu_MainCategories"));
	menuLogic.AddVertex(INVENTORY);
	panels.emplace_back(windowFac.CreateGoldDisplay());
	menuLogic.AddVertex(COINS);

	menuLogic.AddVertex(STATS);
}

void Map_Window_Menu::InitializeLogicGraph()
{
	using enum Map_Window_Menu::MenuWindows;
	menuLogic.AddEdge(MAIN, INVENTORY);
	menuLogic.AddEdge(MAIN, STATS);
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

		if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
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
	int buttonClicked = panels[currentActivePanel]->GetLastClick();
	int nextPanelID = menuLogic.At(currentActivePanel).edges[buttonClicked].destination;

	if (nextPanelID >= panels.size())
	{
		bInStatsMenu = true;
	}
	else
	{
		panelHistory.push(currentActivePanel);
		currentActivePanel = nextPanelID;
		panels[currentActivePanel]->Start();
		bInStatsMenu = false;
	}
}

void Map_Window_Menu::GoToPreviousPanel()
{
	currentActivePanel = panelHistory.top();
	panelHistory.pop();
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

	iPoint allyPosition(170 - camera.x, i - camera.y + 55);
	iPoint hpBarPosition(140 - camera.x, i - camera.y + 80);

	DrawHPBar(character.battlerTextureID, character.currentHP, character.stats[0], hpBarPosition);

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

void Map_Window_Menu::DrawSingleStat(Battler const& character, BaseStats stat, int x, int y) const
{
	app->fonts->DrawText(
		character.GetStatDisplay(stat),
		iPoint(x, y)
	);
}

void Map_Window_Menu::DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const
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

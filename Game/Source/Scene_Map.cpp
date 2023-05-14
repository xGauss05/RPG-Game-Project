#include "Scene_Map.h"
#include "Render.h"
#include "TextManager.h"
#include "Audio.h"
#include "Log.h"

Scene_Map::Scene_Map(GameParty* party)
{
	SetPlayerParty(party);
}

Scene_Map::Scene_Map(std::string const& newMap, GameParty* party)
	: currentMap(newMap)
{
	SetPlayerParty(party);
}
Scene_Map::Scene_Map(std::string const& newMap, iPoint playerCoords, GameParty* party)
	: currentMap(newMap)
{
	player.SetPosition(playerCoords * 48);
	SetPlayerParty(party);
}

bool Scene_Map::isReady()
{
	return true;
}

void Scene_Map::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFac)
{
	if (currentMap.empty())
	{
		currentMap = "Village";
	}

	if (std::string mapToLoad = currentMap + ".tmx";
		!map.Load(path, mapToLoad))
	{
		LOG("Map %s couldn't be loaded.", mapToLoad);
	}

	this->windowFactory = &windowFac;
	xmlNode = info;
	player.Create();

	auto sceneHash = info.find("Map");
	if (sceneHash == info.end())
	{
		LOG("Map scene not found in XML.");
		return;
	}

	auto scene = sceneHash->second;

	for (auto const& window : scene.children("window"))
	{
		if (auto result = windowFac.CreateWindow(window.attribute("name").as_string());
			result != nullptr)
		{
			if (StrEquals("PauseButtonMenu", window.attribute("name").as_string()))
			{
				windows.push_back(std::move(result));
			}
			else if (StrEquals("PauseMenu", window.attribute("name").as_string()))
			{
				pauseWindow.push_back(std::move(result));
			}
			else if (StrEquals("StatsMenu", window.attribute("name").as_string()))
			{
				statsWindow.push_back(std::move(result));
			}
		}
	}

	random100.param(std::uniform_int_distribution<>::param_type(1, 100));
	random1000.param(std::uniform_int_distribution<>::param_type(1, 1000));

	highDialogueSfx = app->audio->LoadFx("Assets/Audio/Fx/S_Town-NPC-TalkHigh.wav");
	midDialogueSfx = app->audio->LoadFx("Assets/Audio/Fx/S_Town-NPC-TalkMid.wav");
	lowDialogueSfx = app->audio->LoadFx("Assets/Audio/Fx/S_Town-NPC-TalkLow.wav");
	battleStartSfx = app->audio->LoadFx("Assets/Audio/Fx/S_Gameplay-BattleStart.wav");
	waterDropSfx = app->audio->LoadFx("Assets/Audio/Fx/S_Dungeon-WaterDroplet.wav");
	torchSfx = app->audio->LoadFx("Assets/Audio/Fx/S_Dungeon-Torch.wav");
}

std::string Scene_Map::PlayMapBgm(std::string_view name)
{
	std::string musicname = "";

	if (name == "Lab_Inside" || name == "Lab_Exterior")
	{
		musicname = "Assets/Audio/Music/M_Town-Lab.ogg";
	}

	if (name == "Dungeon_Outside" || name == "Dungeon01" ||
		name == "Dungeon02" || name == "Dungeon03")
	{
		musicname = "Assets/Audio/Music/M_Dungeon-Main.ogg";
	}

	if (name == "Base" || name == "Village")
	{
		musicname = "Assets/Audio/Music/M_Town-Village.ogg";
	}

	if (name == "Airport")
	{
		musicname = "Assets/Audio/Music/M_Town-Airport.ogg";
	}

	if (name == "Market")
	{
		musicname = "Assets/Audio/Music/M_Town-Market.ogg";
	}

	return musicname;
}

void Scene_Map::PlayDialogueSfx(std::string_view name)
{
	if (StrEquals("high", name))
		app->audio->PlayFx(highDialogueSfx);

	if (StrEquals("mid", name))
		app->audio->PlayFx(midDialogueSfx);

	if (StrEquals("low", name))
		app->audio->PlayFx(lowDialogueSfx);

}

void Scene_Map::DungeonSfx() 
{
	if (currentMap == "Dungeon_Outside" || currentMap == "Dungeon01" ||
		currentMap == "Dungeon02" || currentMap == "Dungeon03")
	{
		std::mt19937 gen(rd());
		int randomValue = random1000(gen);
		if (randomValue <= 2) app->audio->PlayFx(waterDropSfx);

		randomValue = random1000(gen);

		if (randomValue <= 1) app->audio->PlayFx(torchSfx);
	}
}

void Scene_Map::Start()
{
	app->audio->PlayMusic(PlayMapBgm(currentMap).c_str());

	std::vector<std::pair<std::string_view, int>> locationVisited;
	locationVisited.emplace_back(currentMap, 1);

	playerParty->PossibleQuestProgress(QuestType::VISIT, locationVisited, std::vector<std::pair<int, int>>());
}

void Scene_Map::SetPlayerParty(GameParty* party)
{
	playerParty = party;
}

void Scene_Map::Draw()
{
	map.Draw();
	player.Draw();

	for (auto const& elem : windows)
	{
		elem->Draw();
	}

	if (godMode)
	{
		DebugDraw();
	}

	if (statusOpen) DrawStatsMenu();
}

void Scene_Map::DebugItems()
{
	LOG("player HP = %i", playerParty->party[0].currentHP);
	if (app->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_DOWN)
	{
		playerParty->party[0].SetCurrentHP(10);
	}
	if (app->input->GetKey(SDL_SCANCODE_K) == KeyState::KEY_DOWN)
	{
		playerParty->UseItemOnMap(0, 1);
	}
}

void Scene_Map::DebugQuests()
{
	if (app->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_DOWN)
	{
		playerParty->AcceptQuest(2);
		if (auto it = playerParty->currentQuests.find(2);
			it != playerParty->currentQuests.end())
		{
			it->second->DebugQuests();
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_K) == KeyState::KEY_DOWN)
	{
		if (auto it = playerParty->currentQuests.find(2);
			it != playerParty->currentQuests.end())
		{
			it->second->DebugQuests();
		}
		playerParty->CompleteQuest(2);
	}
}

TransitionScene Scene_Map::Update()
{
	DebugQuests();

	if (app->input->GetKey(SDL_SCANCODE_F10) == KeyState::KEY_DOWN)
	{
		godMode = !godMode;
		if (godMode)
		{
			player.SetSpeed(16);
		}
		else
		{
			player.SetSpeed(8);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN)
		statusOpen = !statusOpen;

	auto playerAction = player.HandleInput();

	if (statusOpen)
	{
		UpdateStatsMenu();
		return TransitionScene::NONE;
	}

	using PA = Player::PlayerAction::Action;

	if (state == MapState::ON_MENU_SELECTION)
	{
		switch (windows.back()->Update())
		{
		case 200: //Yes
		{
			LOG("Said yes");
			currentDialogNode = currentDialogDocument.child("dialog").child(currentDialogNode.attribute("yes").as_string());
			state = MapState::ON_DIALOG;
			windows.pop_back();
			app->tex->Load("Assets/UI/GUI_4x_sliced.png");
			auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
			PlayDialogueSfx(currentDialogDocument.child("dialog").attribute("voicetype").as_string());
			currentPanel->ModifyLastWidgetText(currentDialogNode.attribute("text").as_string());
			break;
		}
		case 201: //No
		{
			LOG("Said no");
			currentDialogNode = currentDialogDocument.child("dialog").child(currentDialogNode.attribute("no").as_string());
			state = MapState::ON_DIALOG;
			windows.pop_back();
			app->tex->Load("Assets/UI/GUI_4x_sliced.png");
			auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
			PlayDialogueSfx(currentDialogDocument.child("dialog").attribute("voicetype").as_string());
			currentPanel->ModifyLastWidgetText(currentDialogNode.attribute("text").as_string());
			break;
		}
		default:
			break;
		}
	}
	else if (app->input->controllerCount <= 0)
	{
		windows.back()->Update();
	}

	if ((playerAction.action & PA::MOVE) == PA::MOVE && state == MapState::NORMAL)
	{
		if (map.IsWalkable(playerAction.destinationTile) && !godMode)
		{
			player.StartAction(playerAction);

			return TryRandomBattle();
		}
		else if (godMode)
		{
			player.StartAction(playerAction);
		}
	}
	else if ((playerAction.action & PA::INTERACT) == PA::INTERACT)
	{
		if (state == MapState::ON_MESSAGE)
		{
			windows.pop_back();
			state = MapState::NORMAL;
		}
		else if (state == MapState::ON_DIALOG)
		{
			auto nextDialogName = currentDialogNode.attribute("next").as_string();
			std::string currentDialogName = currentDialogNode.name();

			if (StrEquals(nextDialogName, "quest")
				&& !playerParty->IsQuestAvailable(currentDialogNode.attribute("questid").as_int()))
			{
				nextDialogName = "end";
			}

			if (StrEquals(currentDialogName, "acceptquest"))
			{
				playerParty->AcceptQuest(currentDialogNode.attribute("questid").as_int());
			}
			
			if (StrEquals(nextDialogName, "end"))
			{
				windows.pop_back();
				state = MapState::NORMAL;
			}
			else if (StrEquals(nextDialogName, "Confirmation"))
			{
				// Create Yes/No window
				windows.emplace_back(windowFactory->CreateWindow("Confirmation"));
				state = MapState::ON_MENU_SELECTION;
			}
			else
			{
				currentDialogNode = currentDialogDocument.child("dialog").child(currentDialogNode.attribute("next").as_string());
				auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
				PlayDialogueSfx(currentDialogDocument.child("dialog").attribute("voicetype").as_string());
				currentPanel->ModifyLastWidgetText(currentDialogNode.attribute("text").as_string());
			}
		}
		else if (state == MapState::NORMAL)
		{
			iPoint checktile = player.GetPosition() + (player.lastDir * (map.GetTileWidth() * 3));

			EventTrigger action = map.TriggerEvent(checktile);

			switch (action.eventFunction)
			{
				using enum EventTrigger::WhatToDo;
				case NO_EVENT:
				{
					break;
				}
				case LOOT:
				{
					if (action.values.empty() || !playerParty)
					{
						break;
					}

					for (auto const& [itemToAdd, amountToAdd] : action.values)
					{
						playerParty->AddItemToInventory(itemToAdd, amountToAdd);
						action.text = AddSaveData(action.text, amountToAdd, itemToAdd);
					}
					[[fallthrough]];
				}
				case SHOW_MESSAGE:
				{
					windows.emplace_back(windowFactory->CreateWindow("Message"));
					auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
					currentPanel->ModifyLastWidgetText(action.text);
					state = MapState::ON_MESSAGE;
					break;
				}
				case DIALOG_PATH:
				{
					if (auto result = currentDialogDocument.load_file(action.text.c_str()); !result)
					{
						LOG("Could not load dialog xml file. Pugi error: %s", result.description());
						break;
					}
					windows.emplace_back(windowFactory->CreateWindow("Message"));

					currentDialogNode = currentDialogDocument.child("dialog").child("message1");
					PlayDialogueSfx(currentDialogDocument.child("dialog").attribute("voicetype").as_string());

					// I dont know what this was doing here 
					//currentDialogNode = currentDialogDocument.child("dialog").child(currentDialogNode.attribute("next").as_string());

					auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
					currentPanel->ModifyLastWidgetText(currentDialogNode.attribute("text").as_string());
					state = MapState::ON_DIALOG;
					break;
				}
				case TELEPORT:
				{
					tpInfo = action;
					return TransitionScene::LOAD_MAP_FROM_MAP;
				}
			}
		}
	}

	if (state == MapState::NORMAL)
	{
		player.Update();
		if (player.FinishedMoving())
		{
			EventTrigger action = map.TriggerFloorEvent(player.GetPosition());
			if (action.eventFunction == EventTrigger::WhatToDo::TELEPORT)
			{
				tpInfo = action;
				return TransitionScene::LOAD_MAP_FROM_MAP;
			}
		}

		if (app->input->controllerCount > 0)
		{
			if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_START) == KeyState::KEY_DOWN)
			{
				dynamic_cast<Window_List*>(pauseWindow.back().get())->ResetHoveredButton();
				app->PauseGame();
			}
		}
		else if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::KEY_DOWN)
		{
			app->PauseGame();
		}
	}

	if (transitionTo == TransitionScene::MAIN_MENU)
	{
		transitionTo = TransitionScene::NONE;
		return TransitionScene::MAIN_MENU;
	}
	if (transitionTo == TransitionScene::EXIT_GAME)
	{
		return TransitionScene::EXIT_GAME;
	}

	DungeonSfx();

	return TransitionScene::NONE;
}

std::string_view Scene_Map::GetNextMap() const
{
	return tpInfo.text;
}

iPoint Scene_Map::GetTPCoordinates() const
{
	return iPoint(tpInfo.values[0].second, tpInfo.values[1].second);
}

TransitionScene Scene_Map::TryRandomBattle()
{
	if (currentMap == "Village" || currentMap == "Lab_Exterior" || currentMap == "Dungeon_Outside" || 
		currentMap == "Dungeon01" || currentMap == "Dungeon02" || currentMap == "Dungeon03")
	{
		std::mt19937 gen(rd());
		int randomValue = random100(gen);
		if (randomValue <= 3)
		{
			app->audio->PlayFx(battleStartSfx);

			return TransitionScene::START_BATTLE;
		}
	}
	return TransitionScene::NONE;
}

int Scene_Map::OnPause()
{
	for (auto const& elem : pauseWindow)
	{
		if (auto result = elem->Update();
			result != 0)
		{
			if (result == 4) transitionTo = TransitionScene::EXIT_GAME;
			if (result == 7) transitionTo = TransitionScene::MAIN_MENU;
			return result;
		}
	}

	for (auto const& elem : pauseWindow)
	{
		elem->Draw();
	}

	return 0;
}

int Scene_Map::CheckNextScene()
{
	return 0;
}

bool Scene_Map::SaveScene(pugi::xml_node const& info)
{
	pugi::xml_node data = info;
	auto currentNode = data.append_child("player");
	currentNode.append_attribute("x").set_value(player.GetPosition().x);
	currentNode.append_attribute("y").set_value(player.GetPosition().y);
	currentNode.append_attribute("currentMap").set_value(currentMap.c_str());

	return false;
}

bool Scene_Map::LoadScene(pugi::xml_node const& info)
{
	pugi::xml_node data = info.next_sibling("player");

	player.SetPosition(iPoint{ data.attribute("x").as_int(),
							   data.attribute("y").as_int() });

	currentMap = data.attribute("currentMap").as_string();


	return false;
}

void Scene_Map::DebugDraw()
{
	//Player Hitbox
	SDL_Rect debugPosition = { player.position.x, player.position.y + player.size.y / 2, player.size.x, player.size.y / 2 };
	app->render->DrawShape(debugPosition, true, SDL_Colour(0, 255, 0, 100));

	//Map Hitboxes

	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };
	iPoint cameraSize = { app->render->GetCamera().w, app->render->GetCamera().h };

	SDL_Rect renderView{};
	renderView.x = -camera.x - 48;
	renderView.y = -camera.y - 48;
	renderView.w = -camera.x + cameraSize.x + map.GetTileWidth();
	renderView.h = -camera.y + cameraSize.y + map.GetTileHeight();

	for (int y = 0; y < map.GetHeight(); y++)
	{
		for (int x = 0; x < map.GetWidth(); x++)
		{
			if (x * 48 > renderView.y && x * 48 < renderView.h &&
				y * 48 > renderView.x && y * 48 < renderView.w)
			{
				if (!map.IsWalkable(iPoint{ y * 48,x * 48 }))
				{
					SDL_Rect rect = { (y * 48), x * 48,48,48 };
					app->render->DrawShape(rect, true, SDL_Colour(255, 0, 0, 100));
				}
			}
		}
	}

	//Text display
	app->fonts->DrawText("GOD MODE ON", TextParameters(0, DrawParameters(0, iPoint{ 20,30 })));

	app->fonts->DrawText("Player pos X: " + std::to_string(player.position.x), TextParameters(0, DrawParameters(0, iPoint{ 40,120 })));
	app->fonts->DrawText("Player pos Y: " + std::to_string(player.position.y), TextParameters(0, DrawParameters(0, iPoint{ 40,160 })));

	std::string mapState = "Unknown";
	switch (state)
	{
	case Scene_Map::MapState::NORMAL:
		mapState = "Normal";
		break;
	case Scene_Map::MapState::ON_MESSAGE:
		mapState = "On Message";
		break;
	case Scene_Map::MapState::ON_DIALOG:
		mapState = "On Dialog";
		break;
	case Scene_Map::MapState::ON_MENU_SELECTION:
		mapState = "On Menu Selection";
		break;
	default:
		mapState = "Unknown";
		break;
	}
	app->fonts->DrawText("Map state: " + mapState, TextParameters(0, DrawParameters(0, iPoint{ 40,230 })));
}

void Scene_Map::DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const
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

void Scene_Map::DrawPlayerStats(PartyCharacter const& character, int i) const
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

void Scene_Map::DrawSingleStat(PartyCharacter const& character, BaseStats stat, int x, int y) const
{
	app->fonts->DrawText(
		character.GetStatDisplay(stat),
		iPoint(x, y)
	);
}

void Scene_Map::DrawStatsMenu()
{
	for (auto const& elem : statsWindow)
	{
		elem->Draw();
	}

	for (int i = 0; auto const& character : playerParty->party)
	{
		DrawPlayerStats(character, i);

		i += 140;
	}

}

void Scene_Map::UpdateStatsMenu()
{
	for (auto const& elem : statsWindow)
	{
		elem->Update();
	}
}
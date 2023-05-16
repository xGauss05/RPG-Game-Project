#include "Scene_Map.h"
#include "Render.h"
#include "TextManager.h"
#include "Audio.h"
#include "Log.h"

Scene_Map::Scene_Map(GameParty* party)
{
	SetPlayerParty(party);
	app->audio->RemoveAllFx();
}

Scene_Map::Scene_Map(std::string const& newMap, GameParty* party)
	: currentMap(newMap)
{
	SetPlayerParty(party);
	app->audio->RemoveAllFx();
}
Scene_Map::Scene_Map(std::string const& newMap, iPoint playerCoords, GameParty* party)
	: currentMap(newMap)
{
	spawnPlayerPosition = playerCoords * 48;
	SetPlayerParty(party);
	app->audio->RemoveAllFx();
}

void Scene_Map::SpawnPlayerPosition()
{
	if(!spawnPlayerPosition.IsZero())
	{
		player.SetPosition(spawnPlayerPosition);
		spawnPlayerPosition = { 0, 0 };
	}
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
	auto publisherParty = static_cast<Publisher*>(playerParty);

	if (std::string mapToLoad = currentMap + ".tmx";
		!map.Load(path, mapToLoad, *publisherParty))
	{
		LOG("Map %s couldn't be loaded.", mapToLoad);
	}

	windowFactory = &windowFac;
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
		}
	}

	mainMenu = std::make_unique<Map_Window_Menu>(windowFac);
	mainMenu->SetPlayerParty(playerParty);
	mainMenu->Start();

	questLog = windowFac.CreateQuestLog();
	questLog->SetPlayerParty(playerParty);
	questLog->Start();

	random100.param(std::uniform_int_distribution<>::param_type(1, 100));
	random1000.param(std::uniform_int_distribution<>::param_type(1, 1000));

	highDialogueSfx = app->audio->LoadFx("Fx/S_Town-NPC-TalkHigh.wav");
	midDialogueSfx = app->audio->LoadFx("Fx/S_Town-NPC-TalkMid.wav");
	lowDialogueSfx = app->audio->LoadFx("Fx/S_Town-NPC-TalkLow.wav");
	battleStartSfx = app->audio->LoadFx("Fx/S_Gameplay-BattleStart.wav");
	waterDropSfx = app->audio->LoadFx("Fx/S_Dungeon-WaterDroplet.wav");
	torchSfx = app->audio->LoadFx("Fx/S_Dungeon-Torch.wav");

	SubscribeEventsToGlobalSwitches();

	auto & [triggerAction, id, st] = globalSwitchWaiting;

	if (id != -1 && triggerAction != EventProperties::GlobalSwitchOnInteract::NONE)
	{
		switch (triggerAction)
		{
			using enum EventProperties::GlobalSwitchOnInteract;
		case SET:
			playerParty->SetGlobalSwitchState(id, st);
			break;
		case TOGGLE:
			playerParty->ToggleGlobalSwitchState(id);
			break;
		default:
			break;
		}

		triggerAction = EventProperties::GlobalSwitchOnInteract::NONE;
		id = -1;
		st = false;
	}
}

std::string Scene_Map::PlayMapBgm(std::string_view name)
{
	std::string musicname = "";

	if (name == "Lab_Inside" || name == "Lab_Exterior")
	{
		musicname = "Music/M_Town-Lab.ogg";
	}

	if (name == "Dungeon_Outside" || name == "Dungeon01" ||
		name == "Dungeon02" || name == "Dungeon03")
	{
		musicname = "Music/M_Dungeon-Main.ogg";
	}

	if (name == "Base" || name == "Village")
	{
		musicname = "Music/M_Town-Village.ogg";
	}

	if (name == "Airport")
	{
		musicname = "Music/M_Town-Airport.ogg";
	}

	if (name == "Market")
	{
		musicname = "Music/M_Town-Market.ogg";
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

void Scene_Map::SubscribeEventsToGlobalSwitches()
{
	map.SubscribeEventsToGlobalSwitches();
}

void Scene_Map::Start()
{
	app->audio->PlayMusic(PlayMapBgm(currentMap).c_str());

	std::vector<std::pair<std::string_view, int>> locationVisited;
	locationVisited.emplace_back(currentMap, 1);

	playerParty->PossibleQuestProgress(QuestType::VISIT, locationVisited, std::vector<std::pair<int, int>>());

	SpawnPlayerPosition();

	app->render->AdjustCamera(player.GetPosition());
}

void Scene_Map::SetPlayerParty(GameParty* party)
{
	playerParty = party;
}

void Scene_Map::Draw()
{
	map.Draw();
	player.Draw();

	if (godMode)
	{
		DebugDraw();
	}

	for (auto const& elem : windows)
	{
		elem->Draw();
	}

	if (state == MapState::ON_MENU)
	{
		mainMenu->Draw();
	}
	
	if(state != MapState::ON_MENU)
	{
		questLog->Draw();
	}
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

void Scene_Map::DebugInventory()
{
	if (app->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_DOWN)
	{
		DebugAddALLItemsWithRandomAmounts();
	}
}

void Scene_Map::DebugAddALLItemsWithRandomAmounts()
{
	std::mt19937 gen(rd());
	std::uniform_int_distribution random99(1, 99);

	for (auto const& item : playerParty->dbItems->items)
	{
		if (&item == std::to_address(playerParty->dbItems->items.begin()))
			continue;

		playerParty->AddItemToInventory(item.id, random99(gen));
	}
}

TransitionScene Scene_Map::Update()
{
	DebugInventory();
	if (state == MapState::NORMAL)
	{
		if(playerParty->GetUpdateQuestLog())
		{
			questLog->UpdateQuests();
			if (playerParty->IsQuestMessagePending())
			{
				windows.emplace_back(windowFactory->CreateWindow("Message"));
				auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
				currentPanel->ModifyLastWidgetText(playerParty->QuestCompleteMessage());
				state = MapState::ON_MESSAGE;
			}
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KeyState::KEY_DOWN)
	{
		godMode = !godMode;
		if (godMode)
		{
			player.SetSpeed(32);
		}
		else
		{
			player.SetSpeed(16);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN)
	{
		using enum Scene_Map::MapState;
		if(state == NORMAL)
		{
			lastState = state;
			state = ON_MENU;
			mainMenu->Start();
		}
		else if (state == ON_MENU)
		{
			state = lastState;
			state = NORMAL;
		}
	}

	if (state == MapState::ON_MENU)
	{
		if (!mainMenu->Update())
		{
			state = lastState;
			state = MapState::NORMAL;
		}
		return TransitionScene::NONE;
	}

	auto playerAction = player.HandleInput();

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
		else
		{
			player.RotatePlayer();
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

			std::string altDialogName = currentDialogName;
			altDialogName.pop_back();

			std::string altNextDialogName = nextDialogName;
			altNextDialogName.pop_back();

			if ((StrEquals(nextDialogName, "quest")
				&& !playerParty->IsQuestAvailable(currentDialogNode.attribute("questid").as_int()))
				|| (StrEquals(altNextDialogName, "quest")
					&& !playerParty->IsQuestAvailable(currentDialogNode.attribute("questid").as_int())))
			{
				bool messageFound = false;

				for (auto auxDialogNode = currentDialogNode; auxDialogNode.next_sibling(); auxDialogNode = auxDialogNode.next_sibling())
				{
					std::string innerLoopName = auxDialogNode.next_sibling().name();
					innerLoopName.pop_back();

					if (StrEquals(auxDialogNode.next_sibling().name(), "questnotavailable") || StrEquals(innerLoopName, "questnotavailable"))
					{
						messageFound = true;
						break;
					}
				}

				if (!messageFound)
				{
					nextDialogName = "end";
					messageFound = false;
				}
			}


			if (StrEquals(currentDialogName, "acceptquest") || StrEquals(altDialogName, "acceptquest"))
			{
				playerParty->AcceptQuest(currentDialogNode.attribute("questid").as_int());
			}
			
			if (StrEquals(nextDialogName, "combat"))
			{
				app->audio->PlayFx(battleStartSfx);
				nextFightName = currentDialogNode.attribute("fightname").as_string();
				currentDialogNode = currentDialogDocument.child("dialog").child("victory");

				globalSwitchWaiting = std::make_tuple(
					EventProperties::GlobalSwitchOnInteract::SET,
					currentDialogDocument.child("dialog").attribute("globalswitch").as_int(),
					currentDialogDocument.child("dialog").attribute("value").as_bool()
				);

				return TransitionScene::START_BATTLE;
			}
			if (StrEquals(nextDialogName, "end"))
			{
				windows.pop_back();
				state = MapState::NORMAL;
			}
			else if (StrEquals(nextDialogName, "confirmation"))
			{
				// Create Yes/No window
				windows.emplace_back(windowFactory->CreateWindow("Confirmation"));
				state = MapState::ON_MENU_SELECTION;
			}
			else
			{
				if(!StrEquals(currentDialogNode.name(), "victory"))
				{
					currentDialogNode = currentDialogDocument.child("dialog").child(currentDialogNode.attribute("next").as_string());
				}

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
						if (StrEquals(itemToAdd, "Coins"))
						{
							playerParty->AddGold(amountToAdd);
							if (amountToAdd == 1)
								action.text = AddSaveData(action.text, amountToAdd, "coin");
							else
								action.text = AddSaveData(action.text, amountToAdd, "coins");

						}
						else
						{
							playerParty->AddItemToInventory(itemToAdd, amountToAdd);
							action.text = AddSaveData(action.text, amountToAdd, itemToAdd);

						}

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

					if (std::string npcName = currentDialogDocument.child("dialog").attribute("name").as_string();
						!npcName.empty())
					{
						std::vector<std::pair<std::string_view, int>> npcTalkedTo;
						npcTalkedTo.emplace_back(npcName, 1);

						playerParty->PossibleQuestProgress(QuestType::TALK_TO, npcTalkedTo, std::vector<std::pair<int, int>>());

					}

					windows.emplace_back(windowFactory->CreateWindow("Message"));

					currentDialogNode = currentDialogDocument.child("dialog").child("message1");
					PlayDialogueSfx(currentDialogDocument.child("dialog").attribute("voicetype").as_string());

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
				case GLOBAL_SWITCH:
				{
					for (auto& it = action.globalSwitchIteratorBegin; it != action.globalSwitchIteratorEnd; ++it)
					{
						switch (it->functionOnInteract)
						{
							using enum EventProperties::GlobalSwitchOnInteract;
						case SET:
						{
							playerParty->SetGlobalSwitchState(it->id, it->setTo);
							break;
						}
						case TOGGLE:
						{
							playerParty->ToggleGlobalSwitchState(it->id);
							break;
						}
						default:
						{
							break;
						}
						}
					}
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

std::string_view Scene_Map::GetFightName() const
{
	return nextFightName;
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

	spawnPlayerPosition = { data.attribute("x").as_int(), data.attribute("y").as_int() };

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

	app->fonts->DrawText("GOD MODE ON", iPoint( 20,40 ));
	app->fonts->DrawText(std::format("Player pos X: {}", player.position.x), iPoint(40, 120));
	app->fonts->DrawText(std::format("Player pos Y: {}", player.position.y), iPoint(40, 160));

	std::string mapState = "Unknown";
	switch (state)
	{
	using enum Scene_Map::MapState;
	case NORMAL:
		mapState = "Normal";
		break;
	case ON_MESSAGE:
		mapState = "On Message";
		break;
	case ON_DIALOG:
		mapState = "On Dialog";
		break;
	case ON_MENU_SELECTION:
		mapState = "On Menu Selection";
		break;
	default:
		mapState = "Unknown";
		break;
	}
	app->fonts->DrawText("Map state: " + mapState, TextParameters(0, DrawParameters(0, iPoint{ 40,230 })));
}

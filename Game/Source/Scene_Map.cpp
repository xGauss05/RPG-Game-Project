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

	using enum AvailableSFXs;
	sfx[DIALOGUE_HIGH] = app->audio->LoadFx("Fx/S_Town-NPC-TalkHigh.wav");
	sfx[DIALOGUE_MID] = app->audio->LoadFx("Fx/S_Town-NPC-TalkMid.wav");
	sfx[DIALOGUE_LOW] = app->audio->LoadFx("Fx/S_Town-NPC-TalkLow.wav");
	sfx[BATTLE_START] = app->audio->LoadFx("Fx/S_Gameplay-BattleStart.wav");
	sfx[WATER_DROP] = app->audio->LoadFx("Fx/S_Dungeon-WaterDroplet.wav");
	sfx[TORCH] = app->audio->LoadFx("Fx/S_Dungeon-Torch.wav");

	SubscribeEventsToGlobalSwitches();
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

void Scene_Map::PlaySFX(int id) const
{
	app->audio->PlayFx(id);
}

// TODO: Move this to NPC_Generic.cpp
void Scene_Map::PlayDialogueSfx(std::string_view name)
{
	if (StrEquals("high", name))
	{
		PlaySFX(sfx[AvailableSFXs::DIALOGUE_HIGH]);
		return;
	}

	if (StrEquals("mid", name))
	{
		PlaySFX(sfx[AvailableSFXs::DIALOGUE_MID]);
		return;
	}

	if (StrEquals("low", name))
	{
		PlaySFX(sfx[AvailableSFXs::DIALOGUE_LOW]);
		return;
	}

}

// TODO: Move this to Map.cpp (and turn it on depending on *.tmx properties)
void Scene_Map::DungeonSfx() 
{
	if (currentMap == "Dungeon_Outside" || currentMap == "Dungeon01" ||
		currentMap == "Dungeon02" || currentMap == "Dungeon03")
	{
		std::mt19937 gen(rd());
		int randomValue = random1000(gen);
		if (randomValue <= 2)
		{
			PlaySFX(sfx[AvailableSFXs::WATER_DROP]);
		}

		randomValue = random1000(gen);

		if (randomValue <= 1)
		{
			PlaySFX(sfx[AvailableSFXs::TORCH]);
		}
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

	auto& [triggerAction, id, st] = globalSwitchWaiting;

	/*if (id != -1 && triggerAction != EventProperties::GlobalSwitchOnInteract::NONE)
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
	}*/

	if (id != 1) { playerParty->SetGlobalSwitchState(id, st);         
	globalSwitchWaiting = std::tuple(EventProperties::GlobalSwitchOnInteract::NONE, -1, false); }
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

void Scene_Map::CreateMessageWindow(std::string_view message, MapState newState)
{
	windows.emplace_back(windowFactory->CreateWindow("Message"));

	ModifyLastWidgetMessage(message);

	state = newState;
}

void Scene_Map::ModifyLastWidgetMessage(std::string_view message)
{
	auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
	currentPanel->ModifyLastWidgetText(message);
}

void Scene_Map::StateNormal_HandleInput()
{
	// Toggle God Mode if F10 is pressed
	if (app->input->GetKey(SDL_SCANCODE_F10) == KeyState::KEY_DOWN)
	{
		godMode = !godMode;
		player.SetSpeed(godMode ? 32 : 16);
	}

	// Open character menu if C is pressed
	if (app->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN)
	{
		lastState = state;
		state = MapState::ON_MENU;
		mainMenu->Start();
	}

	// Pause game on Escape press
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::KEY_DOWN)
	{
		app->PauseGame();
	}
}

void Scene_Map::StateMenu_HandleInput()
{
	// Close character window if 
	if (app->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN)
	{
		state = lastState;
		state = MapState::NORMAL;
	}
}

TransitionScene Scene_Map::Update()
{
	auto playerAction = player.HandleInput();
	using PA = Player::PlayerAction::Action;

	switch (state)
	{
	using enum MapState;
	case NORMAL:
	{
		// Update quest log (and tracker) if needed
		if (playerParty->GetUpdateQuestLog())
		{
			questLog->UpdateQuests();
			if (playerParty->IsQuestMessagePending())
			{
				CreateMessageWindow(playerParty->QuestCompleteMessage());
			}
		}

		if ((playerAction.action & PA::MOVE) == PA::MOVE)
		{
			if (map.IsWalkable(playerAction.destinationTile))
			{
				player.StartAction(playerAction);
				CheckRandomBattle();
			}
			else
			{
				player.RotatePlayer();
			}
		}
		else if ((playerAction.action & PA::INTERACT) == PA::INTERACT)
		{
			iPoint checktile = player.GetPosition() + (player.lastDir * (map.GetTileWidth() * 3));
			EventTrigger action = map.TriggerEvent(checktile);

			switch (action.eventFunction)
			{
				using enum EventTrigger::WhatToDo;
				case LOOT:
				{
					if (!playerParty)
					{
						break;
					}

					for (auto const& [itemToAdd, amountToAdd] : action.values)
					{
						if (StrEquals(itemToAdd, "Coins")) [[unlikely]]
						{
							playerParty->AddGold(amountToAdd);

							action.text = (amountToAdd == 1)
								? AddSaveData(action.text, amountToAdd, "coin")
								: AddSaveData(action.text, amountToAdd, "coins");
						}
						else [[likely]]
						{
							playerParty->AddItemToInventory(itemToAdd, amountToAdd);
							action.text = AddSaveData(action.text, amountToAdd, itemToAdd);
						}
					}
				}
				case SHOW_MESSAGE:
				{
					CreateMessageWindow(action.text);
					break;
				}
				case DIALOG_PATH:
				{
					if (auto result = currentDialogDocument.load_file(action.text.c_str());
						!result)
					{
						LOG("Could not load dialog xml file. Pugi error: %s", result.description());
						break;
					}

					auto const& dialogNode = currentDialogDocument.child("dialog");

					// Check if player has any quest of "TALK_TO" type, and if it does, update progress.
					if (std::string npcName = dialogNode.attribute("name").as_string();
						!npcName.empty())
					{
						std::vector<std::pair<std::string_view, int>> npcTalkedTo;
						npcTalkedTo.emplace_back(npcName, 1);

						playerParty->PossibleQuestProgress(QuestType::TALK_TO, npcTalkedTo, std::vector<std::pair<int, int>>());

					}

					currentDialogNode = dialogNode.child("message1");

					// Create the window that will be used
					CreateMessageWindow(currentDialogNode.attribute("text").as_string(), MapState::ON_DIALOG);

					PlayDialogueSfx(dialogNode.attribute("voicetype").as_string());
				}
				case TELEPORT:
				{
					tpInfo = action;
					// TODO: Decide if this stays here or it checks if tpInfo.empty() at the end of post-update.
					return TransitionScene::LOAD_MAP_FROM_MAP;
				}
				case GLOBAL_SWITCH:
				{
					for (auto& it = action.globalSwitchIteratorBegin; it != action.globalSwitchIteratorEnd; ++it)
					{
						using enum EventProperties::GlobalSwitchOnInteract;
						if (it->functionOnInteract == SET)
						{
							playerParty->SetGlobalSwitchState(it->id, it->setTo);
						}
						else if (it->functionOnInteract == TOGGLE)
						{
							playerParty->ToggleGlobalSwitchState(it->id);
						}
					}
				}
				case NO_EVENT:
				{
					break;
				}
			}
		}

		player.Update();

		if (player.IsStandingStill())
		{
			EventTrigger action = map.TriggerFloorEvent(player.GetPosition());
			if (action.eventFunction == EventTrigger::WhatToDo::TELEPORT)
			{
				tpInfo = action;
				return TransitionScene::LOAD_MAP_FROM_MAP;
			}
		}
	}
	case ON_MENU:
	{
		break;
	}
	case ON_MENU_SELECTION:
	{
		auto const &dialogNode = currentDialogDocument.child("dialog");

		if (int buttonClicked = windows.back()->Update();
			buttonClicked == 200 || buttonClicked == 201)
		{
			// Remove Yes/No confirmation window
			windows.pop_back();

			// After a confimation screen ALWAYS goes a dialog.
			state = MapState::ON_DIALOG;

			// Play corresponding SFX dialogue voice
			PlayDialogueSfx(dialogNode.attribute("voicetype").as_string());

			// Check if yes or no was clicked
			std::string optionClicked = buttonClicked == 200 ? "yes" : "no";

			// We get the value of attribute "next" so we know the node we have to jump to.
			// It will always be a message.
			auto const nextNodeName = currentDialogNode.attribute(optionClicked.c_str()).as_string();
			currentDialogNode = dialogNode.child(nextNodeName);

			// Modify the text on the message window.
			ModifyLastWidgetMessage(currentDialogNode.attribute("text").as_string());
		}
	}
	case ON_MESSAGE:
	{
		if ((playerAction.action & PA::INTERACT) == PA::INTERACT)
		{
			windows.pop_back();
			state = MapState::NORMAL;
		}
	}
	}

	
	if (app->input->controllerCount <= 0)
	{
		windows.back()->Update();
	}

	if ((playerAction.action & PA::MOVE) == PA::MOVE && state == MapState::NORMAL)
	{
		
	}
	else if ((playerAction.action & PA::INTERACT) == PA::INTERACT)
	{
		if (state == MapState::ON_DIALOG)
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
	}

	if (state == MapState::NORMAL)
	{

		if (app->input->controllerCount > 0)
		{
			if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_START) == KeyState::KEY_DOWN)
			{
				dynamic_cast<Window_List*>(pauseWindow.back().get())->ResetHoveredButton();
				app->PauseGame();
			}
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
		currentMap == "Dungeon01" || currentMap == "Dungeon02")
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

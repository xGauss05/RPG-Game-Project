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
		currentMap = "AAAAAA";
	}
	auto publisherParty = static_cast<Publisher*>(playerParty);

	if (std::string mapToLoad = currentMap + ".tmx";
		!map.Load(path, mapToLoad, *publisherParty))
	{
		LOG("Map %s couldn't be loaded.", mapToLoad);
	}

	windowFactory = &windowFac;
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

//TODO: Move this to map properties
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

	if (auto const &[triggerAction, id, st] = globalSwitchWaiting;
		id != 1)
	{
		playerParty->SetGlobalSwitchState(id, st);         
	
		globalSwitchWaiting = std::tuple(EventProperties::GlobalSwitchOnInteract::NONE, -1, false);
	}
}

void Scene_Map::SetPlayerParty(GameParty* party)
{
	playerParty = party;
}

void Scene_Map::Draw()
{
	map.Draw();
	player.Draw();
	map.RedrawBelowPlayer(player.position);
	map.DrawLastLayer();

	if (godMode)
	{
		DebugDraw();
	}

	if (state == MapState::ON_MENU)
	{
		mainMenu->Draw();
	}
	
	if(state != MapState::ON_MENU)
	{
		questLog->Draw();
	}

	for (auto const& elem : windows)
	{
		elem->Draw();
	}
}

void Scene_Map::DebugLevels()
{
	if (app->input->GetKey(SDL_SCANCODE_2) == KeyState::KEY_DOWN)
	{
		for (auto& elem : playerParty->party)
		{
			elem.SetLevel(elem.level + 1);
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_3) == KeyState::KEY_DOWN)
	{
		for (auto& elem : playerParty->party)
		{
			elem.AddXP(20);
			LOG("XP To next level = %i", elem.GetXPToNextLevel());
		}
	}
}

void Scene_Map::DebugItems()
{
	if (app->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_DOWN)
	{
		for (auto& elem : playerParty->party)
		{
			elem.SetCurrentHP(10);
		}
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
		int newSpeed = godMode ? 16 : 16;
		player.SetSpeed(newSpeed);
	}

	// Open character menu if C is pressed
	if (IsMenuInputPressed())
	{
		state = MapState::ON_MENU;
		mainMenu->Start();
	}

	// Pause game on Escape press
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::KEY_DOWN)
	{
		app->PauseGame();
	}
}

void Scene_Map::UpdateNormalMapState(Player::PlayerAction playerAction)
{
	using PA = Player::PlayerAction;

	StateNormal_HandleInput();

	// Update quest log (and tracker) if needed
	if (playerParty->GetUpdateQuestLog())
	{
		StateNormal_HandleQuestUpdates();
	}

	if (player.MovedToNewTileThisFrame())
	{
		if (!StateNormal_HandlePlayerNewTile())
		{
			return;
		}
	}

	if (playerAction == PA::MOVE)
	{
		player.StartOrRotateMovement(map.IsWalkable(player.GetTileInFront()));
	}
	else if (playerAction == PA::INTERACT)
	{
		StateNormal_HandlePlayerInteract();
	}

	player.Update();
}

bool Scene_Map::StateNormal_HandlePlayerNewTile()
{
	player.NewTileChecksDone();

	EventTrigger action = map.TriggerPlayerTouchEvent(player.GetPosition());

	if (action.eventFunction == EventTrigger::WhatToDo::TELEPORT)
	{
		tpInfo = action;
		transitionTo = TransitionScene::LOAD_MAP_FROM_MAP;
		return false;
	}

	if (CheckRandomBattle())
	{
		transitionTo = TransitionScene::START_BATTLE;
		return false;
	}

	return true;
}

void Scene_Map::StateNormal_HandleQuestUpdates()
{
	questLog->UpdateQuests();
	if (playerParty->IsQuestMessagePending())
	{
		CreateMessageWindow(playerParty->QuestCompleteMessage());
	}
}

void Scene_Map::StartNewDialog(EventTrigger const& action)
{
	if (auto result = currentDialogDocument.load_file(action.text.c_str());
			!result)
	{
		LOG("Could not load dialog xml file. Pugi error: %s", result.description());
		return;
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

	if (std::string sfxToPlay = dialogNode.attribute("voicetype").as_string();
		!sfxToPlay.empty())
	{
		PlayDialogueSfx(sfxToPlay);
	}
}

void Scene_Map::StateNormal_HandlePlayerInteract()
{
	EventTrigger action = map.TriggerActionButtonEvent(player.GetTileInFront());

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
			[[fallthrough]];
		}
		case SHOW_MESSAGE:
		{
			CreateMessageWindow(action.text);
			break;
		}
		case DIALOG_PATH:
		{
			StartNewDialog(action);
			break;
		}
		case TELEPORT:
		{
			tpInfo = action;
			transitionTo = TransitionScene::LOAD_MAP_FROM_MAP;
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
			break;
		}
		case NO_EVENT:
		{
			break;
		}
	}
}

bool Scene_Map::IsMenuInputPressed() const
{
	return (app->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN
		|| app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN);
}

TransitionScene Scene_Map::Update()
{
	DebugLevels();
	DebugItems();
	DebugInventory();
	if (app->input->GetKey(SDL_SCANCODE_1) == KeyState::KEY_DOWN)
	{
		for (auto &elem : playerParty->party)
		{
			if(!elem.IsDead())
				elem.SetCurrentHP(elem.currentHP - 1);
		}
	}
	map.Update();

	auto playerAction = player.HandleInput();
	using PA = Player::PlayerAction;

	switch (state)
	{
		using enum MapState;
		case NORMAL:
		{
			UpdateNormalMapState(playerAction);
			break;
		}
		case ON_MENU:
		{
			if (!mainMenu->Update())
			{
				state = MapState::NORMAL;
			}
			break;
		}
		case ON_MENU_SELECTION:
		{
			auto const& dialogNode = currentDialogDocument.child("dialog");

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
			break;
		}
		case ON_MESSAGE:
		{
			if (playerAction == PA::INTERACT)
			{
				windows.pop_back();
				state = MapState::NORMAL;
			}
			break;
		}
		case ON_DIALOG:
		{
			if (playerAction == PA::INTERACT)
			{
				auto const& dialogNode = currentDialogDocument.child("dialog");

				std::string_view nextDialogName = currentDialogNode.attribute("next").as_string();
				std::string_view currentDialogName = currentDialogNode.name();

				// If the next node is quest, and, quest with quest id is not available -> end dialog.
				if (StrEquals(nextDialogName, "quest")
					&& !playerParty->IsQuestAvailable(currentDialogNode.attribute("questid").as_int()))
				{
					nextDialogName = "end";
				}

				// If we've accepted the quest via jumping to accept quest node, we add the quest to the party
				if (StrEquals(currentDialogName, "acceptquest"))
				{
					playerParty->AcceptQuest(currentDialogNode.attribute("questid").as_int());
				}

				if (StrEquals(nextDialogName, "combat"))
				{
					PlaySFX(sfx[AvailableSFXs::BATTLE_START]);

					nextFightName = currentDialogNode.attribute("fightname").as_string();

					currentDialogNode = dialogNode.child("victory");

					globalSwitchWaiting = std::make_tuple(
						EventProperties::GlobalSwitchOnInteract::SET,
						dialogNode.attribute("globalswitch").as_int(),
						dialogNode.attribute("value").as_bool()
					);

					return TransitionScene::START_BATTLE;
				}
				else if (StrEquals(nextDialogName, "end"))
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
					if (!StrEquals(currentDialogNode.name(), "victory"))
					{
						currentDialogNode = dialogNode.child(currentDialogNode.attribute("next").as_string());
					}

					PlayDialogueSfx(dialogNode.attribute("voicetype").as_string());

					ModifyLastWidgetMessage(currentDialogNode.attribute("text").as_string());
				}
			}
			break;
		}
	}

	if (transitionTo != TransitionScene::NONE)
	{
		auto retValue = transitionTo;
		transitionTo = TransitionScene::NONE;
		return retValue;
	}

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

bool Scene_Map::CheckRandomBattle()
{
	if (map.AreThereEnemyEncounters())
	{
		std::mt19937 gen(rd());
		int randomValue = random100(gen);
		if (randomValue <= 3)
		{
			PlaySFX(sfx[AvailableSFXs::BATTLE_START]);

			return true;
		}
	}
	return false;
};

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
	SDL_Rect debugPosition = { player.GetDrawPosition().x, player.GetDrawPosition().y, player.size.x, player.size.y};
	app->render->DrawShape(debugPosition, true, SDL_Color(0, 255, 255, 100));
	debugPosition = { player.GetPosition().x, player.GetPosition().y, player.size.x, map.GetTileHeight()};
	app->render->DrawShape(debugPosition, true, SDL_Color(0, 255, 0, 100));

	//Map Hitboxes

	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };
	iPoint cameraSize = { app->render->GetCamera().w, app->render->GetCamera().h };

	SDL_Rect renderView{};
	renderView.x = -camera.x - map.GetTileWidth() + 2;
	renderView.y = -camera.y - map.GetTileHeight() + 2;
	renderView.w = -camera.x + cameraSize.x + map.GetTileWidth() - 2;
	renderView.h = -camera.y + cameraSize.y + map.GetTileHeight() - 2;

	for (int y = 0; y < map.GetHeight(); y++)
	{
		for (int x = 0; x < map.GetWidth(); x++)
		{
			if (x * map.GetTileHeight() > renderView.y && x * map.GetTileHeight() < renderView.h &&
				y * map.GetTileWidth() > renderView.x && y * map.GetTileWidth() < renderView.w)
			{
				if (!map.IsWalkable(iPoint{ y * map.GetTileWidth() ,x * map.GetTileHeight() }))
				{
					SDL_Rect rect = {
						y * map.GetTileWidth(),
						x * map.GetTileHeight(),
						map.GetTileWidth(),
						map.GetTileHeight()
					};
					app->render->DrawShape(rect, false, SDL_Color(255, 0, 0, 100));
				}
			}
		}
	}

	iPoint debugTextPosition = { 40, 0 };
	iPoint mousePosition = app->input->GetMousePosition();

	std::string debugTextMessage = "GOD MODE ON";
	DrawDebugTextLine(debugTextMessage, debugTextPosition);

	debugTextMessage = "Reminder: Position is bottom square.";
	DrawDebugTextLine(debugTextMessage, debugTextPosition);

	debugTextMessage = std::format("Player Position: \"{},{}\"", player.GetPosition().x, player.GetPosition().y);
	DrawDebugTextLine(debugTextMessage, debugTextPosition);

	debugTextMessage = std::format("Player Coordinates: \"{},{}\"", player.GetPosition().x / map.GetTileWidth(), player.GetPosition().y / map.GetTileHeight());
	DrawDebugTextLine(debugTextMessage, debugTextPosition);

	mousePosition = { -app->render->GetCamera().x + mousePosition.x, -app->render->GetCamera().y + mousePosition.y };

	debugTextMessage = std::format("Mouse World Position: \"{},{}\"", mousePosition.x, mousePosition.y);
	DrawDebugTextLine(debugTextMessage, debugTextPosition);

	debugTextMessage = std::format("Mouse Coordinates: \"{},{}\"", mousePosition.x / map.GetTileWidth(), mousePosition.y / map.GetTileHeight());
	DrawDebugTextLine(debugTextMessage, debugTextPosition);


	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
	{
		player.SetPosition(map.MapToWorld(map.WorldToMap(mousePosition)));
	}

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

	debugTextMessage = std::format("Map state is {}", mapState);
	DrawDebugTextLine(debugTextMessage, debugTextPosition);
}

void Scene_Map::DrawDebugTextLine(std::string_view text, iPoint& pos) const
{
	pos.y += 40;
	app->fonts->DrawText(text, pos);
}

#include "Scene_Map.h"
#include "Audio.h"
#include "Log.h"


bool Scene_Map::isReady()
{
	return true;
}

void Scene_Map::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFac)
{
	// Load map
	currentMap = "Base";

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
		}
	}

	app->audio->PlayMusic("Assets/Audio/Music/bgm_placeholder.ogg");
	exit =  false;
}

void Scene_Map::Start()
{
	app->tex->Load("Assets/UI/GUI_4x_sliced.png");
}

void Scene_Map::Draw()
{
	map.Draw();
	player.Draw();
	for (auto const& elem : windows)
	{
		elem->Draw();
	}
}

TransitionScene Scene_Map::Update()
{
	auto playerAction = player.HandleInput();

	using PA = Player::PlayerAction::Action;

	if ((playerAction.action & PA::MOVE) == PA::MOVE && state == MapState::NORMAL)
	{
		if (map.IsWalkable(playerAction.destinationTile))
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
			auto nextDialogNode = currentDialogNode.attribute("next").as_string();

			if (StrEquals(nextDialogNode, "end"))
			{
				windows.pop_back();
				state = MapState::NORMAL;
			}
			else if (StrEquals(nextDialogNode, "Confirmation"))
			{
				// Create Yes/No window
			}
			else
			{
				currentDialogNode = currentDialogDocument.child("dialog").child(currentDialogNode.attribute("next").as_string());
				auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
				currentPanel->ModifyLastWidgetText(currentDialogNode.attribute("text").as_string());
			}
		}
		else if (state == MapState::NORMAL)
		{
			iPoint checktile = player.GetPosition() + (player.lastDir * map.GetTileWidth());

			EventTrigger action = map.TriggerEvent(checktile);

			switch (action.eventFunction)
			{
				using enum EventTrigger::WhatToDo;
			case NO_EVENT:
			{
				break;
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
				auto* currentPanel = dynamic_cast<Window_Panel*>(windows.back().get());
				currentPanel->ModifyLastWidgetText(currentDialogNode.attribute("text").as_string());
				state = MapState::ON_DIALOG;
				break;
			}
			case TELEPORT:
			{
				//teleport player
			}
			case LOOT:
			{
				//give items to player
			}
			}
		}
	}

	if(state == MapState::NORMAL) player.Update();
	
	using enum TransitionScene;
	for (auto const& elem : windows)
	{
		switch (elem->Update())
		{
		case 1:
			return NEW_GAME;
		case 2:
			return CONTINUE_GAME;
		case 3:
			// Create new options window
			break;
		case 4:
			return EXIT_GAME;
		default:
			continue;
		}
	}

	if (transitionTo == TransitionScene::MAIN_MENU)
	{
		transitionTo = TransitionScene::NONE;
		return TransitionScene::MAIN_MENU;
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

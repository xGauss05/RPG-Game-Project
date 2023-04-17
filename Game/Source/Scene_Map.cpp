#include "Scene_Map.h"
#include "Audio.h"
#include "Log.h"

Scene_Map::Scene_Map(std::string const& newMap) : currentMap(newMap) {}

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
		}
	}

	app->audio->PlayMusic("Assets/Audio/Music/M_Town-Base.ogg");

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
				app->tex->Load("Assets/UI/GUI_4x_sliced.png");
				
			}
			else if (StrEquals(nextDialogNode, "Confirmation"))
			{
				// Create Yes/No window
				windows.emplace_back(windowFactory->CreateWindow("Confirmation"));
				state = MapState::ON_MENU_SELECTION;
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
			iPoint checktile = player.GetPosition() + (player.lastDir * (map.GetTileWidth() * 3));

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

	if (state == MapState::NORMAL)
	{
		player.Update();

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

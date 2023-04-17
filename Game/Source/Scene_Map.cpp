#include "Scene_Map.h"
#include "Render.h"
#include "TextManager.h"
#include "Audio.h"
#include "Log.h"

Scene_Map::Scene_Map(std::string const& newMap) : currentMap(newMap) {}
Scene_Map::Scene_Map(std::string const& newMap, iPoint playerCoords)
	: currentMap(newMap)
{
	player.SetPosition(playerCoords * 48);
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

	if (godMode)
	{
		DebugDraw();
	}
}

TransitionScene Scene_Map::Update()
{
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
		if (map.IsWalkable(playerAction.destinationTile) && !godMode)
		{
			player.StartAction(playerAction);
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
					tpInfo = action;
					return TransitionScene::LOAD_MAP_FROM_MAP;
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
	app->render->DrawShape(debugPosition, true, SDL_Color(0, 255, 0, 100));

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
					app->render->DrawShape(rect, true, SDL_Color(255, 0, 0, 100));
				}
			}
		}
	}

	//Text display
	app->fonts->DrawText("GOD MODE ON", TextParameters(0, DrawParameters(0, iPoint{ 20,30 })));

	app->fonts->DrawText("Player pos X: " + std::to_string(player.position.x), TextParameters(0, DrawParameters(0, iPoint{40,120})));
	app->fonts->DrawText("Player pos Y: " + std::to_string(player.position.y), TextParameters(0, DrawParameters(0, iPoint{40,160})));

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
	app->fonts->DrawText("Map state: " + mapState, TextParameters(0, DrawParameters(0, iPoint{40,230})));
}
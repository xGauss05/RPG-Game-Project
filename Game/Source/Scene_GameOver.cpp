#include "Scene_GameOver.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Render.h"
#include "Log.h"
#include "TextManager.h"

Scene_GameOver::~Scene_GameOver()
{
	app->tex->Unload(backgroundTexture);
}

bool Scene_GameOver::isReady()
{
	return true;
}

void Scene_GameOver::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFactory)
{
	auto sceneHash = info.find("GameOver");
	if (sceneHash == info.end())
	{
		LOG("GameOver scene not found in XML.");
		return;
	}

	auto scene = sceneHash->second;

	for (auto const& window : scene.children("window"))
	{
		if (auto result = windowFactory.CreateWindow(window.attribute("name").as_string());
			result != nullptr)
		{
			if (StrEquals("GameOverMenu", window.attribute("name").as_string()))
			{
				windows.push_back(std::move(result));
			}
		}
	}

	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/gameover_bg.png");
	app->audio->PlayMusic("Assets/Audio/Music/M_Battle-Loose.ogg");

	app->render->ResetCamera();

	start = std::chrono::high_resolution_clock::now();

	for (auto const& elem : windows)
	{
		for (auto const& widg : elem->widgets)
		{
			widg->GuiEasing.SetTotalTime(2.0);
		}
	}
}

void Scene_GameOver::Start()
{
	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/gameover_bg.png");
	app->audio->PlayMusic("Assets/Audio/Music/M_Battle-Loose.ogg");
	
	app->tex->Load("Assets/UI/GUI_4x_sliced.png");
}

void Scene_GameOver::Draw()
{
	app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(0, 0)));

	for (auto const& elem : windows)
	{
		elem->Draw();
	}
}

TransitionScene Scene_GameOver::Update()
{
	using enum TransitionScene;

	for (auto const& elem : windows)
	{
		switch (elem->Update())
		{
		
		case 2:
			return CONTINUE_GAME;
		case 4:
			return EXIT_GAME;
		case 7:
			return MAIN_MENU;
		default:
			continue;
		}
	}

	return NONE;
}

int Scene_GameOver::OnPause()
{
	return 0;
}

int Scene_GameOver::CheckNextScene()
{
	return 1;
}

bool Scene_GameOver::SaveScene(pugi::xml_node const& info)
{
	return false;
}

bool Scene_GameOver::LoadScene(pugi::xml_node const& info)
{
	return false;
}

void Scene_GameOver::DebugDraw()
{
	return;
}

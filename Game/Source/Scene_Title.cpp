#include "Scene_Title.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Render.h"
#include "Log.h"
#include "TextManager.h"

Scene_Title::~Scene_Title()
{
	app->tex->Unload(backgroundTexture);
}

bool Scene_Title::isReady()
{
	return true;
}

void Scene_Title::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const &windowFactory)
{
	auto sceneHash = info.find("Title");
	if (sceneHash == info.end())
	{
		LOG("Title scene not found in XML.");
		return;
	}

	auto scene = sceneHash->second;

	for (auto const& window : scene.children("window"))
	{
		if (auto result = windowFactory.CreateWindow(window.attribute("name").as_string());
			result != nullptr)
		{
			if (StrEquals("MainMenu", window.attribute("name").as_string()))
			{
				windows.push_back(std::move(result));
			}
			else if (StrEquals("OptionsMenu", window.attribute("name").as_string()))
			{
				optionsWindow.push_back(std::move(result));
			}
		}
	}

	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/title_bg.png");
	app->audio->PlayMusic("Assets/Audio/Music/M_Menu-Music.ogg");
	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Menu-Title.wav");
	playedLogo = false;
	
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

void Scene_Title::Start()
{
	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/title_bg.png");
	app->audio->PlayMusic("Assets/Audio/Music/M_Menu-Music.ogg");
	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Menu-Title.wav");
	app->tex->Load("Assets/UI/GUI_4x_sliced.png");
}

void Scene_Title::Draw()
{
	app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(0, 0)));
	if (app->scene->options) {

		for (auto const& elem : optionsWindow)
		{
			elem->Draw();
		}
		app->fonts->DrawText("Options", TextParameters(0, DrawParameters(0, iPoint{ 555,190 })));
		app->fonts->DrawText("Fullscreen", TextParameters(0, DrawParameters(0, iPoint{ 425,255 })));
		app->fonts->DrawText("VSync", TextParameters(0, DrawParameters(0, iPoint{ 425,305 })));
		app->fonts->DrawText("SFX " + std::to_string(app->audio->GetSFXVolume()), TextParameters(0, DrawParameters(0, iPoint{570,385})));
		app->fonts->DrawText("BGM " + std::to_string(app->audio->GetBGMVolume()), TextParameters(0, DrawParameters(0, iPoint{565,475})));
		

	}
	else {
		for (auto const& elem : windows)
		{
			elem->Draw();
		}
	}
	
}

TransitionScene Scene_Title::Update()
{
	if (!playedLogo)
	{
		app->audio->PlayFx(logoFx);
		playedLogo = true;
	}

	DoButtonsEasing();

	using enum TransitionScene;
	if (app->scene->options)
	{
		for (auto const& elem : optionsWindow)
		{
			elem->Update();
		}
	}
	else 
	{
		for (auto const& elem : windows)
		{
			switch (elem->Update())
			{
			case 1:
				return NEW_GAME;
			case 2:
				return CONTINUE_GAME;
			case 4:
				return EXIT_GAME;
			default:
				continue;
			}
		}
	}
	
	return NONE;
}

int Scene_Title::OnPause() 
{
	return 0;
}

int Scene_Title::CheckNextScene()
{
	return 1;
}

bool Scene_Title::SaveScene(pugi::xml_node const& info)
{
	return false;
}

bool Scene_Title::LoadScene(pugi::xml_node const& info)
{
	return false;
}

void Scene_Title::DebugDraw()
{
	return;
}

void Scene_Title::DoButtonsEasing()
{
	current = std::chrono::high_resolution_clock::now();

	std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);

	for (auto const& elem : windows)
	{
		if (elapsed.count() > 1000 && elapsed.count() < 1100)
		{
			elem->widgets.at(0)->GuiEasing.SetFinished(false);
		}
		if (elapsed.count() > 1100 && elapsed.count() < 1200)
		{
			elem->widgets.at(1)->GuiEasing.SetFinished(false);
		}
		if (elapsed.count() > 1200 && elapsed.count() < 1300)
		{
			elem->widgets.at(2)->GuiEasing.SetFinished(false);
		}
		if (elapsed.count() > 1300 && elapsed.count() < 1400)
		{
			elem->widgets.at(3)->GuiEasing.SetFinished(false);
		}

		for (auto const& widg : elem->widgets)
		{
			if (!widg->GuiEasing.GetFinished())
			{
				double t = widg->GuiEasing.TrackTime(app->dt);
				double easedX = widg->GuiEasing.EasingAnimation(widg->GetStartingPosition().x, widg->GetTargetPosition().x, t, EasingType::EASE_OUT_ELASTIC);
				uPoint widgetPosition = widg->GetCurrentPosition();
				widgetPosition.x = easedX;
				widg->SetPosition(widgetPosition);
			}
		}
	}
}

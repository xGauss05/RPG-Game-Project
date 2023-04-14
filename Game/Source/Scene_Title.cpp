#include "Scene_Title.h"
#include "Audio.h"
#include "Render.h"
#include "Log.h"

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
			windows.push_back(std::move(result));
		}
	}
	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/title_bg.png");
	app->audio->PlayMusic("Assets/Audio/Music/M_Menu-Music.ogg");
	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Menu-Title.wav");
	playedLogo = false;
}

void Scene_Title::Start()
{
	
}

void Scene_Title::Draw()
{
	app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(0, 0)));
	for (auto const& elem : windows)
	{
		elem->Draw();
	}
}

TransitionScene Scene_Title::Update()
{
	if (!playedLogo)
	{
		app->audio->PlayFx(logoFx);
		playedLogo = true;
	}

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

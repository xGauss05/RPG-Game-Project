#include "Scene_Boot.h"
#include "Audio.h"
#include "Render.h"
#include "Log.h"

Scene_Boot::~Scene_Boot()
{
	app->tex->Unload(backgroundTexture);
}

bool Scene_Boot::isReady()
{
	return true;
}

void Scene_Boot::Load(std::string const& path, LookUpXMLNodeFromString const& info, Window_Factory const& windowFactory)
{
	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/title_bg.png");
	
	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Menu-Title.wav");
	playedLogo = false;
}

void Scene_Boot::Start()
{
}

void Scene_Boot::Draw()
{
	app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(0, 0)));
}

TransitionScene Scene_Boot::Update()
{
	if (!playedLogo)
	{
		app->audio->PlayFx(logoFx);
		playedLogo = true;
	}

	return TransitionScene::NONE;
}

int Scene_Boot::OnPause()
{
	return 0;
}

int Scene_Boot::CheckNextScene()
{
	return 1;
}

bool Scene_Boot::SaveScene(pugi::xml_node const&)
{
	return false;
}

bool Scene_Boot::LoadScene(pugi::xml_node const&)
{
	return false;
}

void Scene_Boot::DebugDraw()
{
	return;
}

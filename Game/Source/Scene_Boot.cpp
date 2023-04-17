#include "Scene_Boot.h"
#include "Audio.h"
#include "Render.h"

#include "Defs.h"
#include "Log.h"

#include <chrono>
#include <stdlib.h>

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
	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/boot_bg.png");

	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Menu-Title.wav");
	playedLogo = false;
	start = std::chrono::high_resolution_clock::now();
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
	
	currentTime = std::chrono::high_resolution_clock::now();

	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - start);

	if (elapsed >= std::chrono::milliseconds(3000)) return TransitionScene::MAIN_MENU;
	
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

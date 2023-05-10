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
	studioTexture = app->tex->Load("Assets/Textures/Backgrounds/logo_not_that_pocho_studios.png");

	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Boot-Logo.wav");
	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Boot-Logo.wav");
	playedLogo = false;
	start = std::chrono::high_resolution_clock::now();

	app->render->AddEasing(1.0f);
}

void Scene_Boot::Start()
{
}

void Scene_Boot::Draw()
{
	//app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(0, 0)));
	SDL_Rect rect = { 0,0,1280,720 };
	app->render->DrawShape(rect, true, SDL_Color(255, 255, 255, 255));

	DoImagesEasing();
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

void Scene_Boot::DebugDraw()
{
	return;
}

void Scene_Boot::DoImagesEasing()
{
	app->render->DrawEasing(studioTexture, iPoint(400, -215), iPoint(400, 160), 0, EasingType::EASE_OUT_QUAD);
}

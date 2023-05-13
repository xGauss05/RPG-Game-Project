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
	presentsTexture = app->tex->Load("Assets/Textures/Backgrounds/boot_presents.png");

	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Boot-Logo.wav");
	logoFx = app->audio->LoadFx("Assets/Audio/Fx/S_Boot-Logo.wav");
	playedLogo = false;
	start = std::chrono::high_resolution_clock::now();

	app->render->InitEasings(info.find("Boot")->second.parent().child("easings"));
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
	currentTime = std::chrono::high_resolution_clock::now();

	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - start);

	if (elapsed >= std::chrono::milliseconds(500) && elapsed <= std::chrono::milliseconds(600))
	{
		app->render->StartEasing("StudioLogo");
	}
	if (elapsed >= std::chrono::milliseconds(1300) && elapsed <= std::chrono::milliseconds(1400))
	{
		app->render->StartEasing("PresentsText");
	}
	if (!playedLogo && app->render->EasingHasFinished("PresentsText"))
	{
		app->audio->PlayFx(logoFx);
		playedLogo = true;
	}

	if (elapsed >= std::chrono::milliseconds(6000)) return TransitionScene::MAIN_MENU;
	
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
	app->render->DrawEasing(studioTexture, "StudioLogo");
	app->render->DrawEasing(presentsTexture, "PresentsText");
}

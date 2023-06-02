#include "Scene_Title.h"
#include "SceneManager.h"
#include "Audio.h"
#include "Render.h"
#include "Log.h"
#include "TextManager.h"
#include "Easing.h"

// FIXME: Remove #include "SceneManager.h".
// No reason for the managed class to have access to the manager itself.

Scene_Title::~Scene_Title()
{
	app->tex->Unload(backgroundTexture);
	app->tex->Unload(titleTexture);
	app->tex->Unload(studioTexture);
	app->audio->RemoveFx(logoFx);
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

	playedLogo = false;
	
	app->render->ResetCamera();

	start = std::chrono::high_resolution_clock::now();

	for (auto const& elem : windows)
	{
		for (auto const& widg : elem->widgets)
		{
			widg->GuiEasing.SetTotalTime(2.0);
			widg->SetHasEasing(true);
		}
	}

	app->render->InitEasings(sceneHash->second.parent().child("easings"));
}

void Scene_Title::Start()
{
	backgroundTexture = app->tex->Load("Assets/Textures/Backgrounds/title_bg.png");
	titleTexture = app->tex->Load("Assets/Textures/Backgrounds/logo_return.png");
	studioTexture = app->tex->Load("Assets/Textures/Backgrounds/logo_not_that_pocho_studios.png");
	// WTF? WHY ARE WE LOADING DATA WITHOUT STORING ANY REFERENCE SO WE CAN CLEAN UP LATER.
	// THIS IS A TICKING BOMB.
	// WHY? BECAUSE WE JUST LOAD EVERYTHING EVERYWHERE NON-STOP 
	// DUE TO THE TECHNICAL DEBT FROM THE FIRST DELIVERY
	// SO WE CANT MAKE SURE THIS WILL FREE.
	// TODO: FIX THIS MEMORY LEAK.
	// 
	// --- But on the positive side, it's not leaking right now due to the lines someone comented on
	// audio.cpp. Soooooooooooooo, we can leave it as it is and never look at it again.
	// 
	// Wait a fucking second.
	// Now that I'm looking at the rest of the file.
	// We are not unloading jackshit.
	// and doing two loads of everything for no reason whatsoever.
	// what.
	app->audio->PlayMusic("Music/M_Menu-Music.ogg");
	logoFx = app->audio->LoadFx("Fx/S_Menu-Title.wav");
}

void Scene_Title::Draw()
{
	app->render->DrawTexture(DrawParameters(backgroundTexture, iPoint(0, 0)));

	DoImagesEasing();
	DoButtonsEasing();

	// App has an SceneManager object.
	// SceneManager **manages scenes**.
	// Scene can access scene manager through app.
	// This is how spaghetti code starts.
	// 
	// Not only that, but the name "options" is not good.
	// It doesn't indicate anywhere that it's a boolean that
	// is turned on when options are open in scene **TITLE**.
	// 
	// This should be a member variable of this Scene_Title.
	// Options window is not an scene, there's no business to do with the manager.
	//
	// Also, in case this variable should be of SceneManager.
	// Put it in private and create a getter. After all, the one that is currently
	// opening and closing options window is the scene manager. Noone else should
	// be able to modify it.
	if (app->scene->options) {

		for (auto const& elem : optionsWindow)
		{
			elem->Draw();
		}
		app->fonts->DrawText("Options", iPoint(555, 190));
		app->fonts->DrawText("Fullscreen", iPoint(425, 255));
		app->fonts->DrawText("VSync", iPoint(425, 305));
		app->fonts->DrawText(std::format("SFX {}", app->audio->GetSFXVolume()), iPoint(570, 385));
		app->fonts->DrawText(std::format("BGM {}", app->audio->GetBGMVolume()), iPoint(565, 475));
		

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
		// Move this to the Start() function so we don't need the check every frame?
		// Not sure if this is used anywhere else
		app->audio->PlayFx(logoFx);
		playedLogo = true;
	}

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
		for (int timeForStart = 1000; auto const &widget : elem->widgets)
		{
			// If it doesn't have easing, we do nothing
			if (!widget->HasEasing())
			{
				continue;
			}

			// If enough time has passed
			if (elapsed.count() > timeForStart)
			{
				// If it hasn't started, we start it
				if (!widget->GuiEasing.GetStarted())
				{
					widget->GuiEasing.Start();
				}
				// If it's already started and it's not finished, we modify position
				else if(!widget->GuiEasing.GetFinished())
				{
					double easedX = widget->GuiEasing.EasingAnimation(
						widget->GetStartingPosition().x,
						widget->GetTargetPosition().x,
						widget->GuiEasing.TrackTime(app->dt),
						EasingType::EASE_OUT_ELASTIC
					);

					widget->SetPosition(
						static_cast<uint>(easedX),
						widget->GetCurrentPosition().y
					);
				}
			}
			
			timeForStart += 100;
		}
	}
}
void Scene_Title::DoImagesEasing()
{
	current = std::chrono::high_resolution_clock::now();

	std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);

	if (elapsed.count() > 500 && elapsed.count() < 600)
	{
		app->render->StartEasing("Title");
	}
	if (elapsed.count() > 750 && elapsed.count() < 850)
	{
		app->render->StartEasing("Logo");
	}

	app->render->DrawEasing(titleTexture, "Title");
	app->render->DrawEasing(studioTexture, "Logo");
}

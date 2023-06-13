#include "Window_List.h"
#include "SceneManager.h"
#include "Scene_Title.h"
#include "Scene_Map.h"
#include "Audio.h"
#include "Render.h"
#include "Log.h"
#include "Window.h"
#include "GuiBox.h"

Window_List::Window_List(pugi::xml_node const& node) : Window_Base(node)
{
	AddFunctionToMap("NewGame", std::bind_front(&Window_List::NewGame, this));
	AddFunctionToMap("ContinueGame", std::bind_front(&Window_List::ContinueGame, this));
	AddFunctionToMap("OptionsMenu", std::bind_front(&Window_List::OptionsMenu, this));
	AddFunctionToMap("ExitGameFromTitle", std::bind_front(&Window_List::ExitGameFromTitle, this));
	AddFunctionToMap("PauseGame", std::bind_front(&Window_List::PauseGame, this));
	AddFunctionToMap("ResumeGame", std::bind_front(&Window_List::ResumeGame, this));
	AddFunctionToMap("ExitMainMenu", std::bind_front(&Window_List::ExitMainMenu, this));
	AddFunctionToMap("ExitGameFromMap", std::bind_front(&Window_List::ExitGameFromMap, this));
	AddFunctionToMap("ExitOptions", std::bind_front(&Window_List::ExitOptions, this));

	AddFunctionToMap("BattleAttack", std::bind_front(&Window_List::BattleAttack, this));
	AddFunctionToMap("BattleSpecialAttack", std::bind_front(&Window_List::BattleSpecialAttack, this));
	AddFunctionToMap("BattleDefend", std::bind_front(&Window_List::BattleDefend, this));
	AddFunctionToMap("BattleRun", std::bind_front(&Window_List::BattleRun, this));

	AddFunctionToMap("DialogYes", std::bind_front(&Window_List::DialogYes, this));
	AddFunctionToMap("DialogNo", std::bind_front(&Window_List::DialogNo, this));

	AddFunctionToMap("DecreaseSFX", std::bind_front(&Window_List::DecreaseSFX, this));
	AddFunctionToMap("IncreaseSFX", std::bind_front(&Window_List::IncreaseSFX, this));

	AddFunctionToMap("DecreaseBGM", std::bind_front(&Window_List::DecreaseBGM, this));
	AddFunctionToMap("IncreaseBGM", std::bind_front(&Window_List::IncreaseBGM, this));

	AddFunctionToMap("ToggleFullscreen", std::bind_front(&Window_List::ToggleFullscreen, this));
	AddFunctionToMap("ToggleVSync", std::bind_front(&Window_List::ToggleVSync, this));

	CreatePanels(node);
	CreateCheckboxes(node);
	CreateButtons(node);

	if (app->input->controllerCount > 0)
	{
		ControllerHoveringWidget(currentHoveredButton);
	}
}

void Window_List::HandleInput()
{
	if (app->input->controllerCount > 0)
	{
		if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KeyState::KEY_DOWN)
		{
			ControllerHoveringWidget(currentHoveredButton);

			if (currentHoveredButton >= GetNumberWidgets() - 1) [[unlikely]]
			{
				currentHoveredButton = 0;

				auto currentwidget = dynamic_cast<GuiBox*>(widgets[currentHoveredButton].get());
				if (currentwidget != nullptr)
				{
					currentHoveredButton++;
				}
			}
			else [[likely]]
			{
				currentHoveredButton++;
			}

			ControllerHoveringWidget(currentHoveredButton);
		}
		if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_UP) == KeyState::KEY_DOWN)
		{
			ControllerHoveringWidget(currentHoveredButton);

			if (currentHoveredButton <= 0) [[unlikely]]
			{
				currentHoveredButton = GetNumberWidgets() - 1;
			}
			else [[likely]]
			{
				currentHoveredButton--;

				auto currentwidget = dynamic_cast<GuiBox*>(widgets[currentHoveredButton].get());
				if (currentwidget)
					currentHoveredButton = GetNumberWidgets() - 1;
			}

			ControllerHoveringWidget(currentHoveredButton);
		}
	}
	else
	{
	}
}

int Window_List::Update()
{
	HandleInput();
	return UpdateWidgets();
}

void Window_List::ResetHoveredButton()
{
	if (GetNumberWidgets() > 0)
	{
		ControllerHoveringWidget(currentHoveredButton);
		currentHoveredButton = 0;
		ControllerHoveringWidget(currentHoveredButton);
	}
}

int Window_List::NewGame()
{
	LOG("NewGame function called");
	return 1;
}

int Window_List::ContinueGame()
{
	LOG("ContinueGame function called");
	return 2;
}

int Window_List::OptionsMenu()
{
	LOG("OptionsWindow function called");
	app->scene->options = true;

	return 3;
}

int Window_List::ExitGameFromTitle()
{
	LOG("ExitGameFromTitle function called");

	return 4;
}

int Window_List::PauseGame() {
	LOG("PauseGame function called");
	app->pause = true;
	if (!app->PauseGame()) return 4;

	return 0;
}

int Window_List::ResumeGame()
{
	LOG("ResumeGame function called");
	app->pause = false;

	return 6;
}

int Window_List::ExitMainMenu()
{
	LOG("ExitMainMenu function called");

	app->pause = false;

	return 7;
}

int Window_List::ExitGameFromMap()
{
	LOG("ExitGameFromMap function called");
	app->pause = false;

	return 4;
}

int Window_List::ExitOptions()
{
	LOG("ExitOption function called");
	app->scene->options = false;

	return 10;
}

int Window_List::BattleAttack()
{
	return 100;
}

int Window_List::BattleSpecialAttack()
{
	return 101;
}

int Window_List::BattleDefend()
{
	return 102;
}

int Window_List::BattleRun()
{
	return 103;
}

int Window_List::DialogYes()
{
	return 200;
}

int Window_List::DialogNo()
{
	return 201;
}

int Window_List::DecreaseSFX()
{
	LOG("DecreaseSFX function called");

	int sfxVolume = app->audio->GetSFXVolume();
	if (sfxVolume > 0)
	{
		sfxVolume -= 2;

		if (sfxVolume < 0) sfxVolume = 0;

		app->audio->SetSFXVolume(sfxVolume);
	}

	return 15;
}

int Window_List::IncreaseSFX()
{
	LOG("IncreaseSFX function called");

	int sfxVolume = app->audio->GetSFXVolume();
	if (sfxVolume < 128)
	{
		sfxVolume += 2;

		if (sfxVolume > 128) sfxVolume = 128;

		app->audio->SetSFXVolume(sfxVolume);
	}

	return 15;
}

int Window_List::DecreaseBGM()
{
	LOG("DecreaseBGM function called");

	int bgmVolume = app->audio->GetBGMVolume();
	if (bgmVolume > 0)
	{
		bgmVolume -= 2;

		if (bgmVolume < 0) bgmVolume = 0;

		app->audio->SetBGMVolume(bgmVolume);
	}

	return 15;
}

int Window_List::IncreaseBGM()
{
	LOG("IncreaseBGM function called");

	int bgmVolume = app->audio->GetBGMVolume();
	if (bgmVolume < 128)
	{
		bgmVolume += 2;

		if (bgmVolume > 128) bgmVolume = 128;

		app->audio->SetBGMVolume(bgmVolume);
	}

	return 15;
}

int Window_List::ToggleFullscreen()
{
	LOG("ToggleFullscreen function called");

	app->win->ToggleFullscreen();

	return 15;
}

int Window_List::ToggleVSync()
{
	LOG("ToggleVSync function called");

	app->render->ToggleVSync();

	return 15;
}

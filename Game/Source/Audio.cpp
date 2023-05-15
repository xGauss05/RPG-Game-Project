#include "App.h"
#include "Audio.h"

#include "Defs.h"
#include "Log.h"
#include <memory>

// NOTE: Recommended using: Additional Include Directories,
// instead of 'hardcoding' library location path in code logic
#include "SDL/include/SDL.h"
#include "SDL_mixer/include/SDL_mixer.h"

// NOTE: Library linkage is configured in Linker Options
//#pragma comment(lib, "../Game/Source/External/SDL_mixer/libx86/SDL2_mixer.lib")

Audio::Audio() : Module()
{
	name = "audio";
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
	}

	// Load support for the OGG audio format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
	}

	SFXVolume = config.child("sfx").attribute("value").as_int();
	BGMVolume = config.child("bgm").attribute("value").as_int();

	return true;
}

// Called before quitting
bool Audio::CleanUp()
{
	if (!active) return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music) Mix_FreeMusic(music);

	for (auto const& item : fx)
		Mix_FreeChunk(item);

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fadeTime)
{
	if (!active) return false;

	if (music)
	{
		/*if (fadeTime > 0.0f)	Mix_FadeOutMusic(int(fadeTime * 1000.0f));
		else */
		Mix_HaltMusic();

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if (!music)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		return false;
	}

	if (fadeTime > 0.0f)
	{
		if (Mix_FadeInMusic(music, -1, (int)(fadeTime * 1000.0f)) < 0)
		{
			LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
			return false;
		}
	}
	else
	{
		if (Mix_PlayMusic(music, -1) < 0)
		{
			LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
			return false;
		}
	}

	LOG("Successfully playing %s", path);
	return true;
}

// Load WAV
int Audio::LoadFx(const char* path)
{
	if (!active)
		return 0;

	fx.push_back(Mix_LoadWAV(path));
	if (!fx.back())
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
		fx.pop_back();
	}

	return fx.size() - 1;
}

// Play WAV
bool Audio::PlayFx(int id, int repeat)
{
	if (!active)
		return false;

	if (id >= 0 && id < fx.size())
	{
		for (int i = 0; auto const& item : fx)
		{
			if (i == id)
				Mix_PlayChannel(-1, item, repeat);

			++i;
		}
	}

	return true;
}

bool Audio::RemoveFx(int id)
{
	if (id >= 0 && id < fx.size())
	{
		auto eraseIt = fx.begin();
		std::advance(eraseIt, id);
		fx.erase(eraseIt);

		return true;
	}
	return false;
}

void Audio::RemoveAllFx()
{
	for (auto const& item : fx)
		Mix_FreeChunk(item);

	fx.clear();
}

void Audio::SetSFXVolume(int value)
{
	SFXVolume = value;
	Mix_Volume(-1, value);
	app->SaveAttributeToConfig(name, "sfx", "value", std::to_string(SFXVolume));
}

void Audio::SetBGMVolume(int value)
{
	BGMVolume = value;
	Mix_VolumeMusic(value);
	app->SaveAttributeToConfig(name, "bgm", "value", std::to_string(BGMVolume));
}

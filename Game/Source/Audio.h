#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"
#include <memory>
#include <queue>
#include <unordered_map>
#include <map>
#include <set>

constexpr auto DEFAULT_MUSIC_FADE_TIME = 2.0f;

struct _Mix_Music;
struct Mix_Chunk;

class Audio : public Module
{
public:

	Audio();

	// Destructor
	~Audio() final;

	// Called before render is available
	bool Awake(pugi::xml_node&) final;

	// Called before quitting
	bool CleanUp() final;

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	int LoadFx(std::string_view path);

	// Play a previously loaded WAV
	bool PlayFx(int fx, int repeat = 0);

	bool RemoveFx(int id);

	void RemoveAllFx();

	void SetBGMVolume(int value);

	void SetSFXVolume(int value);

	int GetBGMVolume()
	{
		return BGMVolume;
	}

	int GetSFXVolume()
	{
		return SFXVolume;
	}


private:
	struct InfoLoadedSFX
	{
		int id = -1;
		int references = 0;
	};

	std::unordered_map<std::string_view, InfoLoadedSFX> m_PathToLoadedInfo;
	std::unordered_map<int, std::string_view> m_IndexToPath;
	std::map<int, Mix_Chunk*> m_sfxMap;
	std::priority_queue<int, std::vector<int>, std::greater<int>> m_AvailableIndexes;

	_Mix_Music* music = nullptr;

	int BGMVolume;
	int SFXVolume;
};

#endif // __AUDIO_H__
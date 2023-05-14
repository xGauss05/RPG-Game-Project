#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"
#include <memory>

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	int LoadFx(const char* path);

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

	_Mix_Music* music = nullptr;
	std::list<Mix_Chunk*> fx;

	int BGMVolume;
	int SFXVolume;
};

#endif // __AUDIO_H__
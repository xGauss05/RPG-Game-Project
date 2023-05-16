#ifndef __j1ASSETMANAGER_H__
#define __j1ASSETMANAGER_H__

#include "Module.h"

struct SDL_RWops;

class AssetManager : public Module
{
public:

	AssetManager();

	// Destructor
	~AssetManager() override;

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	uint LoadData(const char* file, char** buffer) const;

	bool CreatePath(const char* newDir, const char* mount_point = nullptr);

	bool Exists(const char* file) const;

	SDL_RWops* Load(const char* file) const;

private:

};

#endif // __j1FILESYSTEM_H__
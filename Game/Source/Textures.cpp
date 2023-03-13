#include "App.h"
#include "Render.h"
#include "Textures.h"

#include "Defs.h"
#include "Log.h"

#include <functional>
#include <list>
#include <memory>

#include "SDL_image/include/SDL_image.h"

Textures::Textures() : Module()
{
	name = "textures";
}

// Destructor
Textures::~Textures() = default;

// Called before render is available
bool Textures::Awake(pugi::xml_node& config)
{
	LOG("Init Image library");

	// Load support for the PNG image format
	if(int flags = IMG_INIT_PNG; (IMG_Init(flags) & flags) != flags)
	{
		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
		return false;
	}

	return true;
}

// Called before the first frame
bool Textures::Start()
{
	LOG("Start textures");
	return true;
}

// Called before quitting
bool Textures::CleanUp()
{
	LOG("Freeing textures and Image library");
	textures.clear();
	IMG_Quit();
	return true;
}

// Load new texture from file path
std::shared_ptr<SDL_Texture> Textures::Load(const char* path)
{
	if(SDL_Surface* surface = IMG_Load(path)) 
	{
		auto texture = app->render->LoadTexture(surface);
		SDL_FreeSurface(surface);
		if(texture)
		{
			textures.push_back(std::move(texture));

			return textures.back();
		}
		else LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());

	}	
	else LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());

	return nullptr;
}

// Unload texture
bool Textures::Unload(SDL_Texture const *texture)
{
	for(auto it = textures.begin(); it != textures.end(); it++)
	{
		if ((*it).get() == texture)
		{
			(*it).reset();
			textures.erase(it);
			texture = nullptr;
			return true;
		}
	}

	return false;
}

// Retrieve size of a texture
void Textures::GetSize(SDL_Texture* const texture, uint &width, uint &height) const
{
	SDL_QueryTexture(texture, nullptr, nullptr, (int*) &width, (int*) &height);
}

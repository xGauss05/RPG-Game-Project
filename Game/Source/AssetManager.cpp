#include "App.h"
#include "Log.h"
#include "AssetManager.h"
#include "physfs.h"
#include "SDL/include/SDL.h"

AssetManager::AssetManager() : Module()
{
	name = "asset_manager";

	//Here we initialize the filesystem
	PHYSFS_init(nullptr);

	//This works as a default path
	CreatePath(".");
}

// Destructor
AssetManager::~AssetManager()
{
	//Here we close any files opened via PhysicsFS, blanks the search / write paths,
	//frees memory, and invalidates all of your file handles.
	PHYSFS_deinit();
}

bool AssetManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Asset Manager");

	// Here we set up any archives that will be read from. This
	//adds an archive to the search path (as its name says)

	PHYSFS_addToSearchPath("Assets/Audio.zip", 1);

	return true;
}

bool AssetManager::CreatePath(const char* newDir, const char* mount_point)
{
	PHYSFS_mount(newDir, mount_point, 1); //Here we mount an archive file into the virtual filesystem created by init

	if (PHYSFS_mount(newDir, mount_point, 1) == 0)
	{
		LOG("Error: %s\n", PHYSFS_getLastError());
	}
	else
	{
		return false;
	}

	return true;
}

// Make sure that the file exists
bool AssetManager::Exists(const char* file) const
{
	return PHYSFS_exists(file);
}

// Here we read the file and allocate needed bytes to a buffer and write all data into it
uint AssetManager::LoadData(const char* file, char** buffer) const
{
	uint ret = 0;

	PHYSFS_file* data_file = PHYSFS_openRead(file); //We open and read the file we want to 

	if (data_file)
	{
		auto fileLength = static_cast<uint>(PHYSFS_fileLength(data_file)); //We get the file size to find out how many bytes you need to allocate for the file.
		*buffer = new char[fileLength]; //We allocate memory in a buffer of the size of the file
		auto read = static_cast<uint>(PHYSFS_read(data_file, *buffer, 1, fileLength)); //We read data from a PhysFS filehandle
		if (read != fileLength) //If the file lenght is not the same that the one was readed, there will be an error
		{
			LOG("Error while reading from file %s: %s\n", file, PHYSFS_getLastError());
			if (buffer)
			{
				delete buffer; //We delete the buffer
				buffer = nullptr;
			}
		}
		else
		{
			ret = read; //If everything goes well, we return the size of the data
		}
		PHYSFS_close(data_file); //Now we close the PhysFs handle used to read the data
	}
	else 
	{
		LOG("Error while opening file %s: %s\n", file, PHYSFS_getLastError());
	}
	return ret;
}

// This is will be used to load an image/fx/music from a memory buffer with SDL
SDL_RWops* AssetManager::Load(const char* file) const
{
	char* buffer;
	int size = LoadData(file, &buffer); //We get the size of the data from the previous function

	if (size > 0)
	{
		SDL_RWops* r = SDL_RWFromConstMem(buffer, size); //We prepare a read-only buffer

		return r;
	}
	else
		return nullptr;
}

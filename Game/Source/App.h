#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "Defs.h"

#include <list>
#include <memory>

#include "PugiXml/src/pugixml.hpp"
#include "SDL/include/SDL.h"
#include <format>
#include <string>
#include <string_view>

constexpr auto CONFIG_FILENAME = "config.xml";
constexpr auto SAVE_STATE_FILENAME = "save_game.xml";

// Modules
class Window;
class Input;
class Render;
class TextureManager;
class Audio;
class SceneManager;
class TextManager;

template <typename... Args>
std::string AddSaveData(std::string_view format, Args&&... args)
{
	return std::vformat(format, std::make_format_args(args...));
}

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* mod);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;

	// Getters
	std::string GetTitle() const;
	std::string GetOrganization() const;
	uint GetLevelNumber() const;

	SDL_Texture* GetTexture(int id) const;
	SDL_Renderer* GetRender() const;

	bool AppendFragment(pugi::xml_node target, const char *data) const;
	
	// Saving / Loading
	void LoadGameRequest();
	void ResetLevelRequest();
	void SaveGameRequest();
	void GameSaved();
	bool LoadFromFile();
	bool SaveToFile();
	bool SaveAttributeToConfig(
		std::string const &moduleName, 
		std::string const &node, 
		std::string const &attribute, 
		std::string const &value
	);
	
	// Utils
	bool PauseGame();

	// Modules
	std::unique_ptr<Window> win;
	std::unique_ptr<Input> input;
	std::unique_ptr<Render> render;
	std::unique_ptr<TextureManager> tex;
	std::unique_ptr<Audio> audio;
	std::unique_ptr<SceneManager> scene;
	std::unique_ptr<TextManager> fonts;

private:

	// Load config file
	bool LoadConfig();

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Calls the pause on modules so they can still render
	bool DoPaused();

	int argc;
	char** args;
	std::string title;
	std::string organization;

	std::list<Module*> modules;
	
	pugi::xml_document configFile;
	pugi::xml_node configNode;

	uint frames = 0;
	float dt;

	bool saveGameRequested;
	bool loadGameRequested;
	bool resetLevelRequested;

	uint levelNumber = 1;
};

extern std::unique_ptr<App> app;

#endif	// __APP_H__
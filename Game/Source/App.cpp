#include "App.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "TextureManager.h"
#include "Audio.h"
#include "SceneManager.h"
#include "Map.h"
#include "TextManager.h"

#include "Defs.h"
#include "Log.h"

#include <sstream>
#include <format>
#include <string>
#include <string_view>

// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	input = std::make_unique<Input>();
	win = std::make_unique<Window>();
	render = std::make_unique<Render>();
	tex = std::make_unique<TextureManager>();
	audio = std::make_unique<Audio>();
	fonts = std::make_unique<TextManager>();
	scene = std::make_unique<SceneManager>();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input.get());
	AddModule(win.get());
	AddModule(tex.get());
	AddModule(audio.get());
	AddModule(fonts.get());
	AddModule(scene.get());

	// Render last to swap buffer
	AddModule(render.get());
}

// Destructor
App::~App() = default;

void App::AddModule(Module* mod)
{
	mod->Init();
	modules.push_back(mod);
}

// Called before render is available
bool App::Awake()
{
	if(!LoadConfig()) return false;

	title = configNode.child("app").child("title").child_value(); 

	for(auto const &item : modules)
	{
		if(pugi::xml_node node = configNode.child(item->name.c_str());
		   !item->Awake(node))
		{
			return false;
		}
	}

	return true;
}

// Called before the first frame
bool App::Start()
{
	for(auto const &item : modules)
	{
		if(!item->Start()) return false;
	}

	return true;
}

// Called each loop iteration
bool App::Update()
{
	PrepareUpdate();

	if (input->GetWindowEvent(EventWindow::WE_QUIT)) return false;

	if(!PreUpdate()) return false;
	if(!DoUpdate()) return false;
	if(!PostUpdate()) return false;

	FinishUpdate();

	if (input->GetKey(SDL_SCANCODE_P) == KeyState::KEY_DOWN)
	{
		return PauseGame();
	}

	return true;
}

// Load config from XML file
bool App::LoadConfig()
{
	if (pugi::xml_parse_result parseResult = configFile.load_file("config.xml"); 
		parseResult) 
	{
		configNode = configFile.child("config");
	}
	else
	{
		LOG("Error in App::LoadConfig(): %s", parseResult.description());
		return false;
	}

	return true;
}

// ---------------------------------------------
void App::PrepareUpdate()
{
}

// ---------------------------------------------
void App::FinishUpdate()
{
	if (loadGameRequested) LoadFromFile();
	if (saveGameRequested) SaveToFile();
}

// Call modules before each loop iteration
bool App::PreUpdate()
{
	for(auto const &item : modules)
	{
		if(!item->active) continue;
		if(!item->PreUpdate()) return false;
	}

	return true;
}

// Call modules on each loop iteration
bool App::DoUpdate()
{
	for(auto const &item : modules)
	{
		if(!item->active) continue;
		if(!item->Update(dt)) return false;
	}

	return true;
}

// Call modules after each loop iteration
bool App::PostUpdate()
{
	for(auto const &item : modules)
	{
		if(!item->active) continue;
		if(!item->PostUpdate()) return false;
	}

	return true;
}

// Called before quitting
bool App::CleanUp()
{
	for(auto const &item : modules)
		if(!item->CleanUp()) return false;
	
	return true;
}

// ---------------------------------------
int App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return nullptr;
}

// ---------------------------------------
std::string App::GetTitle() const
{
	return title;
}

// ---------------------------------------
std::string App::GetOrganization() const
{
	return organization;
}

void App::LoadGameRequest()
{
	if(!loadGameRequested)
	{
		loadGameRequested = true;

	}
}

// ---------------------------------------
void App::ResetLevelRequest()
{
	if(!resetLevelRequested)
	{
		resetLevelRequested = true;
	}
}
void App::SaveGameRequest() 
{
	if(!saveGameRequested)
	{
		saveGameRequested = true;
	}
}

void App::GameSaved()
{
	saveGameRequested = false;
}

bool App::LoadFromFile()
{
	pugi::xml_document gameStateFile;

	if (pugi::xml_parse_result result = gameStateFile.load_file("save_game.xml"); !result)
	{
		LOG("Could not load xml file savegame.xml. pugi error: %s", result.description());
		return false;
	}
	
	for(auto const &item : modules)
	{
		if(const auto moduleName = item->name.c_str();
		   !item->LoadState(gameStateFile.child("save_state").child(moduleName)))
		{
			return false;
		}
	}

	loadGameRequested = false;

	return true;
}

// check https://pugixml.org/docs/quickstart.html#modify
bool App::SaveToFile() 
{
	auto saveDoc = std::make_unique<pugi::xml_document>();
	pugi::xml_node saveStateNode = saveDoc->append_child("save_state");
	for(auto const &item : modules)
	{
		if(item->HasSaveData())
		{
			if(auto ret = item->SaveState(saveStateNode); !ret.empty())
				saveStateNode.child("save_state").append_copy(ret);
			else
				LOG("Error saving state of module %s", item->name.c_str());
		}
	}

	return saveDoc->save_file("save_game.xml");
}

bool App::SaveAttributeToConfig(std::string const &moduleName, std::string const &node, std::string const &attribute, std::string const &value)
{
	auto m = moduleName.c_str();
	auto n = node.c_str();
	auto a = attribute.c_str();
	auto v = value.c_str();

	if (moduleName.empty() || node.empty() || attribute.empty())
	{
		LOG("Can't save to config. String is empty");
		return false;
	}

	if (configNode.child(m).empty())
	{
		// If module doesn't exist
		configNode.append_child(m).append_child(n).append_attribute(a).set_value(v);
	}
	else if (configNode.child(m).child(n).empty())
	{
		// If node doesn't exist
		configNode.child(m).append_child(n).append_attribute(a).set_value(v);
	}
	else if (configNode.child(m).child(n).attribute(a).empty())
	{
		// If attribute doesn't exist
		configNode.child(m).child(n).append_attribute(a).set_value(v);
	}
	else
	{
		// If everything exists
		configNode.child(m).child(n).attribute(a).set_value(v);
	}

	if (!configFile.save_file("config.xml"))
	{
		LOG("Can't save config.");
		return false;
	}

	return false;
}

bool App::DoPaused()
{
	// PreUpdate
	int phase = 1;
	input->Pause(phase);
	render->Pause(phase);

	// Update
	phase++;
	scene->Pause(phase);

	// PostUpdate
	phase++;
	render->Pause(phase);

	// FinishUpdate
	phase++;
	FinishUpdate();

	return false;
}

bool App::PauseGame()
{
	using enum KeyState;
	while (input->GetKey(SDL_SCANCODE_P) == KEY_DOWN || input->GetKey(SDL_SCANCODE_P) == KEY_REPEAT)
	{
		DoPaused();
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) return false;
	}
	while (input->GetKey(SDL_SCANCODE_P) == KEY_IDLE || input->GetKey(SDL_SCANCODE_P) == KEY_UP)
	{
		DoPaused();
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) return false;
	}

	return true;
}

uint App::GetLevelNumber() const
{
	return levelNumber;
}

SDL_Texture* App::GetTexture(int id) const
{
	return tex->GetTexture(id);
}

SDL_Renderer* App::GetRender() const
{
	return render->GetRender();
}

bool App::AppendFragment(pugi::xml_node target, const char *data) const
{
	pugi::xml_document doc;
	if(auto result = doc.load_string(data);
	   !result)
	{
		LOG("Error parsing XML: ", result.description());
		return false;
	}

	for(auto const &child : doc)
		target.append_copy(child);

	return true;
}

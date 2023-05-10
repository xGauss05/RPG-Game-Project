#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"

#include "Scene_Base.h"
#include "GameParty.h"

#include "Defs.h"		//StringHash

#include <vector>

class Window_Base;

class SceneManager : public Module
{
public:

	SceneManager();

	// Destructor
	~SceneManager() final;

	// Called before render is available
	bool Awake(pugi::xml_node& config) final;

	// Called before the first frame
	bool Start() final;

	// Called before all Updates
	bool PreUpdate() final;

	bool Pause(int phase) final;

	// Called each loop iteration
	bool Update(float dt) final;

	// Called before all Updates
	bool PostUpdate() final;

	// Called before quitting
	bool CleanUp() final;

	bool HasSaveData() const final;
	bool LoadState(pugi::xml_node const &data) final;
	pugi::xml_node SaveState(pugi::xml_node const &) const final;
	bool options = false;
private:
	void StartBattle(std::string_view troopName = "");

	std::string assetPath;
	std::unique_ptr<Scene_Base> currentScene;
	std::unique_ptr<Scene_Base> sceneOnHold;
	std::unique_ptr<Scene_Base> nextScene;

	std::unique_ptr<Window_Factory> windowFactory;
	LookUpXMLNodeFromString bootInfo;
	LookUpXMLNodeFromString sceneInfo;
	LookUpXMLNodeFromString mapInfo;
	std::unique_ptr<GameParty> party;

	bool CurrentlyMainMenu = true;
	
	int pauseMenuBackground = 0;

	bool loadNextMap = false;
};

#endif // __SCENE_H__
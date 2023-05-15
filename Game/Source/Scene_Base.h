#ifndef __SCENE_BASE_H__
#define __SCENE_BASE_H__

#include "Window_Factory.h"

#include "Render.h"
#include "Log.h"

#include <vector>
#include <memory>

enum class TransitionScene
{
	NONE,
	BOOT_COMPLETE,
	MAIN_MENU,
	NEW_GAME,
	CONTINUE_GAME,
	LOAD_MAP_FROM_MAP,
	START_BATTLE,
	WIN_BATTLE,
	LOSE_BATTLE,
	RUN_BATTLE,
	EXIT_GAME
};

class Scene_Base
{
public:
	Scene_Base() = default;
	virtual ~Scene_Base() = default;

	virtual bool isReady() = 0;
	virtual void Load(
		std::string const& path,
		LookUpXMLNodeFromString const &info,
		Window_Factory const &windowFactory
	) = 0;
	virtual void Start() = 0;
	virtual TransitionScene Update() = 0;
	virtual int OnPause() = 0;
	virtual void Draw() = 0;
	virtual int CheckNextScene() = 0;
	virtual bool SaveScene(pugi::xml_node const&) = 0;
	virtual bool LoadScene(pugi::xml_node const&) = 0;
	virtual void DebugDraw() = 0;
	virtual std::string_view GetFightName() const { return ""; };


	bool bActive = false;
	// TODO Fade-in/Fade-out variables (colour, duration)
	std::vector<std::unique_ptr<Window_Base>> windows;

};


#endif __SCENE_BASE_H__
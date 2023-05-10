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

	void InitEasings(pugi::xml_node const& node)
	{
		app->render->easings.clear();

		for (auto const& easing : node.children("image"))
		{
			Easing temp;
			temp.name = easing.attribute("name").as_string();
			temp.startingPos.x = easing.attribute("initialPosX").as_int();
			temp.startingPos.y = easing.attribute("initialPosY").as_int();
			temp.targetPos.x = easing.attribute("targetPosX").as_int();
			temp.targetPos.y = easing.attribute("targetPosY").as_int();
			temp.SetTotalTime(easing.attribute("duration").as_float());
			//temp.type = easing.attribute("type").as_string();

			app->render->easings.push_back(temp);
		}
	}

	bool bActive = false;
	// TODO Fade-in/Fade-out variables (colour, duration)
	std::vector<std::unique_ptr<Window_Base>> windows;

};


#endif __SCENE_BASE_H__
#ifndef __SCENE_BASE_H__
#define __SCENE_BASE_H__

#include "Window_Factory.h"

#include "Log.h"

#include <vector>
#include <memory>

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
	virtual int Update() = 0;
	virtual void Draw() = 0;
	virtual int CheckNextScene() = 0;

	bool bActive = false;
	// TODO Fade-in/Fade-out variables (colour, duration)
	std::vector<std::unique_ptr<Window_Base>> windows;

};


#endif __SCENE_BASE_H__
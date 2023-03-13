#pragma once

#include "Window_Base.h"

#include <stack>

class Scene_Base
{
public:
	Scene_Base() = default;
	virtual ~Scene_Base() = default;

	virtual bool isReady() = 0;
	virtual void Load(std::string const& path) = 0;
	virtual void Start() = 0;
	virtual void Update() = 0;

	bool bActive = false;
	// TODO Fade-in/Fade-out variables (colour, duration)
	std::stack<Window_Base> windows;

};


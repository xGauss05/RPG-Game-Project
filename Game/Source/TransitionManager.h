#ifndef __TRANSITION_MANAGER_H__
#define __TRANSITION_MANAGER_H__

#include "Module.h"
#include "Scene_Base.h"
#include "Color.h"
#include "Point.h"
#include "Transition.h"

class		Transition;

class TransitionManager :  public Module
{
public:
	TransitionManager();
	~TransitionManager();

	bool PostUpdate();

	bool CleanUp();

public:

	void DeleteActiveTransition();
	
	Transition* SceneToBattle(float step_duration = 1.0f, Color fade_colour = Black);

public:

	bool isTransitioning = false;

private:

	Transition* activeTransition;

};

#endif // !__TRANSITION_MANAGER_H__

#ifndef __TRANSITION_MANAGER_H__
#define __TRANSITION_MANAGER_H__

#include "Module.h"
#include "Scene_Base.h"
#include "Color.h"
#include "Point.h"
//#include "Transition.h"

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
	
	Transition* CreateFadeToColour(Scene_Base* next_scene, float step_duration = 1.0f, Color fade_colour = Black);

public:

	bool is_transitioning;

private:

	Transition* active_transition;

};

#endif // !__TRANSITION_MANAGER_H__

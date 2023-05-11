#include "TransitionManager.h"
#include "FadeToColour.h"

TransitionManager::TransitionManager() : active_transition(nullptr), is_transitioning(false)
{

}

TransitionManager::~TransitionManager()
{

}

bool TransitionManager::PostUpdate()
{
	bool ret = true;

	if (active_transition != nullptr)
	{
		active_transition->StepTransition();
	}

	return ret;
}

bool TransitionManager::CleanUp()
{
	bool ret = true;
	
	if (active_transition != nullptr)
	{
		delete active_transition;
		active_transition = nullptr;
	}

	return ret;
}

void TransitionManager::DeleteActiveTransition()
{
	is_transitioning = false;

	delete active_transition;

	active_transition = nullptr;
}

Transition* TransitionManager::CreateFadeToColour(Scene_Base* next_scene, float step_duration, Color fade_colour)
{	
	if (!is_transitioning)
	{
		active_transition = new FadeToColour(next_scene, step_duration, fade_colour);

		is_transitioning = true;
	}

	return active_transition;
}

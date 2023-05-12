#include "TransitionManager.h"
#include "SceneManager.h"
#include "FadeToColour.h"

TransitionManager::TransitionManager() : activeTransition(nullptr), isTransitioning(false)
{

}

TransitionManager::~TransitionManager()
{

}

bool TransitionManager::PostUpdate()
{
	bool ret = true;

	if (activeTransition != nullptr)
	{
		activeTransition->StepTransition();
	}

	return ret;
}

bool TransitionManager::CleanUp()
{
	bool ret = true;
	
	if (activeTransition != nullptr)
	{
		delete activeTransition;
		activeTransition = nullptr;
	}

	return ret;
}

void TransitionManager::DeleteActiveTransition()
{
	isTransitioning = false;

	delete activeTransition;

	activeTransition = nullptr;
}

Transition* TransitionManager::SceneToBattle(float step_duration, Color fade_colour)
{	
	if (!isTransitioning)
	{
		activeTransition = new FadeToColour(step_duration, fade_colour);

		isTransitioning = true;
	}

	return activeTransition;
}

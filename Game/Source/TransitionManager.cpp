#include "TransitionManager.h"
#include "SceneManager.h"
#include "Transition_FadeToColour.h"

TransitionManager::TransitionManager() = default;

TransitionManager::~TransitionManager() = default;

bool TransitionManager::PostUpdate()
{
	if (isTransitioning && activeTransition)
	{
		activeTransition->StepTransition();
	}

	return true;
}

bool TransitionManager::CleanUp()
{
	return true;
}

void TransitionManager::DeleteActiveTransition()
{
	isTransitioning = false;

	activeTransition.release();
}

void TransitionManager::SceneToBattle(float step_duration, Colour const &fade_colour)
{	
	if (!isTransitioning)
	{
		activeTransition = std::make_unique<FadeToColour>(step_duration, fade_colour);

		isTransitioning = true;
	}
}

bool TransitionManager::IsPastMidpoint() const
{
	if (isTransitioning && activeTransition)
		return activeTransition->IsPastMidpoint();

	return true;
}

bool TransitionManager::IsTransitioning() const
{
	return isTransitioning;
}

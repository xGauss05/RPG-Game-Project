#ifndef __TRANSITION_MANAGER_H__
#define __TRANSITION_MANAGER_H__

#include "Module.h"
#include "Scene_Base.h"
#include "Colour.h"
#include "Point.h"
#include "Transition.h"

class Transition;

class TransitionManager :  public Module
{
public:
	TransitionManager();
	~TransitionManager() override;

	bool PostUpdate() override;

	bool CleanUp() override;

	void DeleteActiveTransition();
	
	void SceneToBattle(float step_duration = 1.0f, Colour  const& fade_colour = g_Colour_Black);

	bool IsPastMidpoint() const;

	bool IsTransitioning() const;

private:

	bool isTransitioning = false;
	std::unique_ptr<Transition> activeTransition;

};

#endif // !__TRANSITION_MANAGER_H__

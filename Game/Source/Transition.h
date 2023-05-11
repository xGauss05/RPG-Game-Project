#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#define MAX_CUTOFF 1.0f
#define MIN_CUTOFF 0.0f

#include "SDL/include/SDL.h"
#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Color.h"
#include "SceneManager.h"

enum class TRANSITION_STEP
{
	NONE,
	ENTERING,
	CHANGING,
	EXITING
};

class Transition
{
public:
	Transition(float step_duration, bool non_lerp = false);
	virtual ~Transition();

	virtual void Start();
	virtual void StepTransition();
	virtual void CleanUp();

public:

	virtual void Entering();
	virtual void Changing();
	virtual void Exiting();

	float Lerp(float start, float end, float rate);
	float N_Lerp(float start, float end, float rate, bool smash_in = false);
	float GetCutoffRate(float step_duration, float dt = app->dt);

public:
	TRANSITION_STEP step = TRANSITION_STEP::NONE;
	std::unique_ptr<Scene_Base> next_scene;

	float step_duration;
	float current_cutoff;

	bool non_lerp;

private:
	float cutoff_rate;
};

#endif // !__TRANSITION_H__
#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Colour.h"
#include "SceneManager.h"
#include "SDL/include/SDL.h"

constexpr auto MAX_CUTOFF = 1.0f;
constexpr auto MIN_CUTOFF = 0.0f;

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
	explicit Transition(float step_duration, bool non_lerp = false);
	virtual ~Transition();

	virtual void Start();
	virtual void StepTransition();
	virtual void CleanUp();

	virtual void Entering();
	virtual void Changing();
	virtual void Exiting();

	float Lerp(float start, float end, float rate) const;
	float N_Lerp(float start, float end, float rate, bool smash_in = false) const;
	float SetCutoffRate(float stepDuration, float dt = app->dt);

	bool IsPastMidpoint() const;

protected:

	TRANSITION_STEP step = TRANSITION_STEP::NONE;

	float step_duration;
	float current_cutoff = 0.0f;

	bool non_lerp;

private:

	float cutoff_rate = 0.0f;
};

#endif // !__TRANSITION_H__
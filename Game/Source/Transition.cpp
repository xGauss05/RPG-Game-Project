#include "Transition.h"

Transition::Transition(float step_duration, bool non_lerp) :
	step_duration(step_duration),
	non_lerp(non_lerp)
{
}

Transition::~Transition() = default;

void Transition::Start()
{
}

void Transition::StepTransition()
{
}

void Transition::CleanUp()
{
}

void Transition::Entering()
{
}

void Transition::Changing()
{
}

void Transition::Exiting()
{
}

float Transition::Lerp(float start, float end, float rate) const
{
	float increment = (end - start) * rate;

	return start + increment;
}

float Transition::N_Lerp(float start, float end, float rate, bool smash_in) const
{
	float increment = 0;

	if (smash_in)
	{
		increment = (end - start) * rate * rate;
	}
	else
	{
		float r = 1 - ((1 - rate) * (1 - rate));
		increment = (end - start) * r;
	}

	return start + increment;
}

float Transition::SetCutoffRate(float stepDuration, float dt)
{
	cutoff_rate = dt / stepDuration;

	return cutoff_rate;
}

bool Transition::IsPastMidpoint() const
{
	return step == TRANSITION_STEP::CHANGING || step == TRANSITION_STEP::EXITING;
}

#include "Transition.h"

Transition::Transition(float step_duration, bool non_lerp) :
	step_duration(step_duration),
	non_lerp(non_lerp),
	cutoff_rate(0.0f),
	current_cutoff(0.0f)
{
}

Transition::~Transition()
{
}

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

float Transition::Lerp(float start, float end, float rate)
{
	float increment = (end - start) * rate;

	return start + increment;
}

float Transition::N_Lerp(float start, float end, float rate, bool smash_in)
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

float Transition::GetCutoffRate(float step_duration, float dt)
{
	cutoff_rate = dt / step_duration;

	return cutoff_rate;
}
#include "Blink.h"
#include "TransitionManager.h"

Blink::Blink(float step_duration, Color color) : Transition(step_duration)
, fade_colour(color)
{
	InitFadeToColour();
}

Blink::~Blink()
{

}

void Blink::StepTransition()
{
	switch (step)
	{
	case TRANSITION_STEP::ENTERING:

		Entering();

		break;

	case TRANSITION_STEP::CHANGING:

		Changing();

		break;

	case TRANSITION_STEP::EXITING:

		Exiting();

		break;
	}

	ApplyFade();
}

void Blink::Entering()
{
	// Blackens
	current_cutoff += GetCutoffRate(step_duration);

	if (current_cutoff >= MAX_CUTOFF)
	{
		current_cutoff = MAX_CUTOFF;

		step = TRANSITION_STEP::CHANGING;
	}
}

void Blink::Changing()
{
	//app->sceneManager->SwitchScene(next_scene);

	if (back2back)
	{
		current_cutoff -= GetCutoffRate(step_duration);

		if (current_cutoff <= MIN_CUTOFF)
		{
			current_cutoff = MIN_CUTOFF;
			back2back = false;
			step = TRANSITION_STEP::CHANGING;
		}
	}
	else
	{
		current_cutoff += GetCutoffRate(step_duration);
		if (current_cutoff >= MAX_CUTOFF)
		{
			current_cutoff = MAX_CUTOFF;

			step = TRANSITION_STEP::EXITING;
		}
	}



}

void Blink::Exiting()
{
	current_cutoff -= GetCutoffRate(step_duration);

	if (current_cutoff <= MIN_CUTOFF)
	{
		current_cutoff = MIN_CUTOFF;

		step = TRANSITION_STEP::NONE;

		app->transition->DeleteActiveTransition();
	}
}

void Blink::ApplyFade()
{
	SDL_SetRenderDrawColor(app->render->GetRender(), fade_colour.r, fade_colour.g, fade_colour.b, current_cutoff * 255.0f);

	SDL_RenderFillRect(app->render->GetRender(), &screen);
}

void Blink::InitFadeToColour()
{
	iPoint windowSize = app->win->GetWindowSize();
	screen = { 0,0,windowSize.x, windowSize.y };

	SDL_SetRenderDrawBlendMode(app->render->GetRender(), SDL_BLENDMODE_BLEND);

	step = TRANSITION_STEP::ENTERING;
}
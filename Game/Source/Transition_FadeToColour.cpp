#include "Transition_FadeToColour.h"
#include "TransitionManager.h"

FadeToColour::FadeToColour(float step_duration, Colour const &colour)
	: Transition(step_duration), fade_colour(colour)
{
	InitFadeToColour();
}

FadeToColour::~FadeToColour() = default;

void FadeToColour::StepTransition()
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

	default:
		break;
	}

	ApplyFade();
}

void FadeToColour::Entering()
{
	// Blackens
	current_cutoff += SetCutoffRate(step_duration);

	if (current_cutoff >= MAX_CUTOFF)
	{
		current_cutoff = MAX_CUTOFF;

		step = TRANSITION_STEP::CHANGING;
	}
}

void FadeToColour::Changing()
{
	current_cutoff -= SetCutoffRate(step_duration);

	if (current_cutoff <= MIN_CUTOFF)
	{
		current_cutoff = MIN_CUTOFF;

	}

	step = TRANSITION_STEP::EXITING;

}

void FadeToColour::Exiting()
{
	current_cutoff -= SetCutoffRate(step_duration);

	if (current_cutoff <= MIN_CUTOFF)
	{
		current_cutoff = MIN_CUTOFF;

		step = TRANSITION_STEP::NONE;

		app->transition->DeleteActiveTransition();
	}
}

void FadeToColour::ApplyFade()
{
	SDL_SetRenderDrawColor(
		app->render->GetRender(),
		fade_colour.r,
		fade_colour.g,
		fade_colour.b,
		static_cast<Uint8>(current_cutoff * 255.0f)
	);

	SDL_RenderFillRect(app->render->GetRender(), &screen);
}

void FadeToColour::InitFadeToColour()
{
	iPoint windowSize = app->win->GetWindowSize();
	screen = { 0, 0, windowSize.x, windowSize.y };

	SDL_SetRenderDrawBlendMode(app->render->GetRender(), SDL_BLENDMODE_BLEND);

	step = TRANSITION_STEP::ENTERING;
}
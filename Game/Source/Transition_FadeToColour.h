#ifndef __FADE_TO_COLOUR_H__
#define __FADE_TO_COLOUR_H__

#include "Transition.h"
#include "Colour.h"

class FadeToColour : public Transition
{
public:
	FadeToColour(float step_duration, Colour const& colour);
	~FadeToColour() override;

	void StepTransition() override;

	void Entering() override;
	void Changing() override;
	void Exiting() override;
	
	void ApplyFade();												

	void InitFadeToColour();										

private:
	SDL_Rect	screen;
	Colour		fade_colour;

};

#endif // !__FADE_TO_COLOUR_H__
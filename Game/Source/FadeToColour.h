#ifndef __FADE_TO_COLOUR_H__
#define __FADE_TO_COLOUR_H__

#include "Transition.h"
#include "Color.h"

class FadeToColour : public Transition
{
public:
	FadeToColour(float step_duration, Color colour);
	~FadeToColour();

	void StepTransition();

public:
	void Entering();												
	void Changing();												
	void Exiting();													
	
	void ApplyFade();												

	void InitFadeToColour();										

private:
	SDL_Rect	screen;
	Color		fade_colour;
};

#endif // !__FADE_TO_COLOUR_H__
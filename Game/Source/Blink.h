#ifndef __BLINK_H__
#define __BLINK_H__

#include "Transition.h"
#include "Color.h"

class Blink : public Transition
{
public:
	Blink(float step_duration, Color color);
	~Blink();

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

	bool back2back = true;

};

#endif // !__BLINK_H__
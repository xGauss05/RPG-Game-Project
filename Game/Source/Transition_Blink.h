#ifndef __BLINK_H__
#define __BLINK_H__

#include "Transition.h"
#include "Colour.h"

class Blink : public Transition
{
public:
	Blink(float step_duration, Colour const& colour);
	~Blink() override;

	void StepTransition() override;

	void Entering() override;												
	void Changing() override;
	void Exiting() override;
	
	void ApplyFade();												

	void InitFadeToColour();										

private:
	SDL_Rect screen;
	Colour fade_colour;

	bool back2back = true;

};

#endif // !__BLINK_H__
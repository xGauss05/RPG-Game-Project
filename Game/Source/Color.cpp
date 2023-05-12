#include "Color.h"
#include "Module.h"

Color GetRandomColour()
{
	Color color = Color((float)(std::rand() % 255), (float)(std::rand() % 255), (float)(std::rand() % 255));

	return color;
}

Color Black = Color(0, 0, 0);
Color White = Color(255, 255, 255);

Color Red		= Color(255, 0, 0);
Color Green		= Color(0, 255, 0);
Color Blue		= Color(0, 0, 255);
Color LowRed	= Color(100, 0, 0);
Color LowGreen	= Color(0, 100, 0);
Color LowBlue	= Color(0, 0, 100);

Color Yellow	= Color(255, 255, 0);
Color Pink		= Color(255, 182, 193);
Color DeepPink	= Color(230, 0, 255);
Color Purple	= Color(128, 64, 128);
Color Orange	= Color(255, 102, 0);
Color Coral		= Color(255, 127, 80);
Color Sienna	= Color(160, 82, 45);
Color SandBrown = Color(244, 164, 96);
Color RosyBrown = Color(188, 143, 143);
Color Magenta	= Color(255, 0, 128);
Color Cyan		= Color(0, 255, 255);
Color Grey		= Color(128, 128, 128);
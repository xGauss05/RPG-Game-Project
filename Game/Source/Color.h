#pragma once

struct Color
{
	float r, g, b, a;

	Color() : r(0), g(0), b(0), a(255)
	{}

	Color(float r, float g, float b, float a = 255) : r(r), g(g), b(b), a(a)
	{}

	void Set(float r, float g, float b, float a = 255)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float* operator & ()
	{
		return (float*)this;
	}

	bool operator ==(const Color& v) const
	{
		return (r == v.r && g == v.g && b == v.b);
	}
};

Color GetRandomColour();

extern Color Black;
extern Color White;

extern Color Red;
extern Color Green;
extern Color Blue;
extern Color LowRed;
extern Color LowGreen;
extern Color LowBlue;

extern Color Yellow;
extern Color Pink;
extern Color LightPink;
extern Color Purple;
extern Color Orange;
extern Color Coral;
extern Color Sienna;
extern Color SandBrown;
extern Color RosyBrown;
extern Color Magenta;
extern Color Cyan;
extern Color Grey;
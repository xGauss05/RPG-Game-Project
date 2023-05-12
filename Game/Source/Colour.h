#pragma once

#include <random>

struct Colour
{
	Colour() = default;

	explicit Colour(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a)
	{}

	Colour(const Colour& other)
	{
		Set(other.r, other.g, other.b, other.a);
	}

	Colour& operator=(const Colour& other)
	{
		Set(other.r, other.g, other.b, other.a);
	}

	~Colour() = default;

	void Set(int red, int green, int blue, int alpha = 255)
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}

	bool operator ==(const Colour& v) const
	{
		return (r == v.r && g == v.g && b == v.b);
	}

	Colour GetRandomColour(bool randomAlpha = false, int alpha = 255)
	{
		std::mt19937 gen(rd());

		random.param(std::uniform_int_distribution<>::param_type(0, 255));

		Colour randomColour(random(gen), random(gen), random(gen));

		randomColour.a = randomAlpha ? random(gen) : alpha;

		return randomColour;

	}

	std::random_device rd;
	std::uniform_int_distribution<> random;

	Uint8 r = 0;
	Uint8 g = 0;
	Uint8 b = 0;
	Uint8 a = 255;
};


extern const Colour g_Colour_Black;
extern const Colour g_Colour_White;
extern const Colour g_Colour_Red;
extern const Colour g_Colour_Green;
extern const Colour g_Colour_Blue;
extern const Colour g_Colour_LowRed;
extern const Colour g_Colour_LowGreen;
extern const Colour g_Colour_LowBlue;
extern const Colour g_Colour_Yellow;
extern const Colour g_Colour_Pink;
extern const Colour g_Colour_DeepPink;
extern const Colour g_Colour_Purple;
extern const Colour g_Colour_Orange;
extern const Colour g_Colour_Coral;
extern const Colour g_Colour_Sienna;
extern const Colour g_Colour_SandBrown;
extern const Colour g_Colour_RosyBrown;
extern const Colour g_Colour_Magenta;
extern const Colour g_Colour_Cyan;
extern const Colour g_Colour_Grey;
#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "App.h"

#include "TextureManager.h"

#include "Point.h"
#include "JSON Parser.h"

#include "PugiXml/src/pugixml.hpp"

class Animation
{
public:
	void Create(std::string_view path, int from = 0, int to = 0)
	{
		JSON_Parser parser;
		parser.load_file(path);

		staticImage = (from == to);

		parser.child("frames").child("Off_0");

		while (from > 0)
		{
			parser.next_sibling();
			from--;
			to--;
		}

		while (to >= 0)
		{
			SDL_Rect rect = { 0, 0, 0, 0 };

			parser.child("spriteSourceSize");

			rect =
			{
				parser.attribute("x").as_int(),
				parser.attribute("y").as_int(),
				parser.attribute("w").as_int(),
				parser.attribute("h").as_int()
			};

			frames.push_back(rect);

			parser.parent();

			frameDuration.emplace_back(parser.attribute("duration").as_int());

			parser.next_sibling();
			to--;
		}
	}

private:
	bool staticImage = true;

	std::vector<SDL_Rect> frames;
	std::vector<int> frameDuration;

	bool finished = false;

	int currentFrame = 0;
	std::chrono::time_point<std::chrono::steady_clock> lastUpdated;
};

class Animated_Sprite
{
public:
	enum class AnimationStates
	{
		STATE_ON,
		STATE_OFF,
		TRIGGER
	};

private:
	std::unordered_map<AnimationStates, Animation, StringHash, std::equal_to<>> animations;
	
	int textureIndex = 0;

	iPoint drawingPosition = { 0, 0 };
};

#endif //__SPRITE_H__

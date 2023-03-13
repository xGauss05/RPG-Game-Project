#ifndef __FONTS_H__
#define __FONTS_H__

#include "Module.h"

#include "Defs.h"
#include "Point.h"

#include <queue>
#include <vector>
#include <unordered_map>

#include "SDL/include/SDL.h"
#include "SDL/include/SDL_pixels.h"

constexpr auto MAX_FONTS = 10;
constexpr auto MAX_FONT_CHARS = 256;

enum class FontDrawNewLine
{
	POSITION_X_ON_MAP,
	POSITION_X_ON_SCREEN,
	NUMBER_OF_CHARS,
	SCREEN_FIT, // Drags text when moving to the right, new lines when moving to the left.
	NONE
};

struct FontCharacter
{
	int xAdvance = 0;
	SDL_Rect rect{0};
	iPoint offset;
};

struct Font
{
	// Key: chararacter || Value: <position in the texture, xAdvance>
	std::unordered_map<char,FontCharacter> fontTable;
	SDL_Texture *graphic = nullptr;
	std::string name = "";
	int lineHeight = 0;
	Point<float> scale = {0,0};
	iPoint spacing = {0,0};
};

class Fonts : public Module
{
public:

	Fonts();
	~Fonts() final;

	bool Awake(pugi::xml_node &config) final;

	// Load Font
	int Load(std::string const &fontName);
	void UnLoad(int font_id);

	// Create a surface from text
	void Draw(
		std::string_view text,
		iPoint position,
		int fontId,
		fPoint scale = {1.0f, 1.0f},
		bool isFixed = true,
		std::pair<FontDrawNewLine, int> maxX = {FontDrawNewLine::NONE, 0},
		SDL_Point pivot = SDL_Point(0, 0),
		double angle = 0.0f
	) const;

	void DrawMiddlePoint(
		std::string_view text,
		iPoint position,
		int fontId,
		fPoint scale = {1.0f, 1.0f},
		bool isFixed = true,
		std::pair<FontDrawNewLine, int> maxX = {FontDrawNewLine::NONE, 0},
		SDL_Point pivot = SDL_Point(0,0),
		double angle = 0.0f
	) const;

private:
	int CheckIfNewLine(int x, int xAdvance, std::pair<FontDrawNewLine, int> maxX, int &i) const;
	
	std::string path;
	std::vector<Font> fonts;
	std::priority_queue<int, std::vector<int>, std::greater<int>> freeVectorElements;

	friend class UI;
};


#endif // __FONTS_H__
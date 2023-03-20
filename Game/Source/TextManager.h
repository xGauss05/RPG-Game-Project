#ifndef __TEXTMANAGER_H__
#define __TEXTMANAGER_H__

#include "Module.h"

#include "Defs.h"
#include "Point.h"

#include <queue>
#include <vector>
#include <unordered_map>

#include "SDL/include/SDL.h"
#include "SDL/include/SDL_pixels.h"

struct DrawParameters;

constexpr auto MAX_FONTS = 10;
constexpr auto MAX_FONT_CHARS = 256;

enum class AlignTo
{
	ALIGN_TOP_LEFT,
	ALIGN_TOP_RIGHT,
	ALIGN_BOTTOM_LEFT,
	ALIGN_BOTTOM_RIGHT,
	ALIGN_CENTER
};

enum class AnchorTo
{
	SCREEN,
	WORLD
};

struct TextParameters
{
	int fontId;
	DrawParameters const &params;

	AnchorTo anchor= AnchorTo::SCREEN;
	AlignTo align = AlignTo::ALIGN_TOP_LEFT;

	TextParameters(int id, DrawParameters const& originalParameters)
		: fontId(id), params(originalParameters) {};

	TextParameters& Align(AlignTo a)
	{
		align = a;
		return *this;
	}
	TextParameters& Anchor(AnchorTo a)
	{
		anchor = a;
		return *this;
	}
};


class TextManager : public Module
{
private:
	struct Font
	{
		struct CharInfo
		{
			int xAdvance = 0;
			SDL_Rect rect{ 0 };
			iPoint offset{ 0 };
		};

		// Key: chararacter || Value: <position in the texture, xAdvance>
		std::unordered_map<char, CharInfo> charMap;
		int textureID = -1;
		std::string name = "";
		int lineHeight = 0;
		fPoint scale = { 1,1 };
		iPoint spacing = { 0,0 };
		void Unload() const;
	};

public:

	TextManager();
	~TextManager() final;

	bool Awake(pugi::xml_node &config) final;
	bool Start() final;

	// Load Font
	int Load(std::string const &fontName);
	void UnLoad(int font_id);

	void DrawText(
		std::string_view text,
		TextParameters const &originalParams
	) const;

private:
	iPoint GetAnchorPosition(iPoint position, AnchorTo anchor) const;
	iPoint GetAlignPosition(std::string_view text, iPoint position, AlignTo align, Font const& font) const;
	int GetDistanceToNextDrawingPositon(int advance, int spacing, int offset, float scale = 1.0f) const;

	std::string path;
	std::vector<Font> fonts;
	std::priority_queue<int, std::vector<int>, std::greater<int>> freeVectorElements;
};


#endif // __TEXTMANAGER_H__
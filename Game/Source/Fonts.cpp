#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Fonts.h"

#include <ranges>
#include <algorithm>
#include <string>
#include <iterator>

// Constructor
Fonts::Fonts() : Module()
{
	name = "fonts";
}

// Destructor
Fonts::~Fonts() = default;

bool Fonts::Awake(pugi::xml_node &config)
{
	path = config.child("property").attribute("path").as_string();
	return true;
}

// Loads the font named fontName with the folder path specified in config.xml.
// Returns:
// new font id, the font id if it was already loaded, or -1 if the texture or xml could not be loaded.
// WARNING: The font has to have a .xml file. If it doesn't, use the other Load function.
int Fonts::Load(std::string const &fontName)
{
	// Check if the font is already loaded
	auto isSameName = [&fontName](Font const &f) { return StrEquals(f.name, fontName); };
		
	if(auto result = std::ranges::find_if(fonts, isSameName);
	   result != fonts.end())
	{
		LOG("Font %s already loaded", fontName.c_str());
		return std::distance(fonts.begin(), result);
	}

	pugi::xml_document fontFile;
	std::string fullName = path + fontName + ".xml";
	if(pugi::xml_parse_result parseResult = fontFile.load_file(fullName.c_str());
	   parseResult)
	{
		Font newFont = {};
		auto fontNode = fontFile.child("font");
		
		std::string texturePath = path + fontNode.child("properties").child("texture").attribute("file").as_string();
		
		if(newFont.graphic = app->tex->Load(texturePath.c_str()).get();
		   !newFont.graphic) 
		{
			return -1;
		}
		
		auto propertiesNode = fontNode.child("properties");
		
		newFont.spacing = {
			propertiesNode.child("info").attribute("xspacing").as_int(),
			propertiesNode.child("info").attribute("yspacing").as_int()
		};
		
		newFont.lineHeight = propertiesNode.child("common").attribute("lineHeight").as_int();
		
		newFont.scale = {
			propertiesNode.child("common").attribute("scaleW").as_float()/100.f,
			propertiesNode.child("common").attribute("scaleY").as_float()/100.f
		};
		
		for(auto const &elem : fontNode.child("chars"))
		{
			FontCharacter newChar = {};
			
			newChar.xAdvance = elem.attribute("xadvance").as_int();
			
			newChar.rect = {
				elem.attribute("x").as_int(),
				elem.attribute("y").as_int(),
				elem.attribute("width").as_int(),
				elem.attribute("height").as_int()
			};

			newChar.offset = {
				elem.attribute("xoffset").as_int(),
				elem.attribute("yoffset").as_int()
			};
			
			newFont.fontTable.try_emplace(
				elem.attribute("letter").as_string()[0],
				newChar
			);
		}
		if(freeVectorElements.empty()) [[likely]]
			fonts.push_back(newFont);
		else [[unlikely]]
		{
			fonts[freeVectorElements.top()] = newFont;
			freeVectorElements.pop();
		}
		LOG("Loaded font %s", fontName.c_str());
	}
	else LOG("Error loading font file %s: %s", fullName.c_str(), parseResult.description());

	return fonts.size() - 1;
}

void Fonts::UnLoad(int font_id)
{
	if(fonts.size() < INT_MAX && in_range(font_id, 0, static_cast<int>(fonts.size())))
	{
		freeVectorElements.emplace(font_id);
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
	else
	{
		LOG("Something went very wrong and fonts vector is too big.");
		LOG("If you see this error, please contact me :(");
	}
}

void Fonts::Draw(std::string_view text, iPoint position, int fontId, fPoint scale, bool isFixed, std::pair<FontDrawNewLine, int> maxX, SDL_Point pivot, double angle) const
{
	if(!in_range(fontId, 0, static_cast<int>(fonts.size())))
	{
		LOG("%s: Invalid font id %d", __func__, fontId);
		return;
	}

	Font const &font = fonts[fontId];
	int xAdvance = 0;
	// 0 = no new line, 1 = new line on space, 2 = new line now
	int newLine = 0;
	
	SDL_Rect camera = app->render->GetCamera();

	// Fix X position if it's a static text on window
	if(isFixed)
	{
		position.x -= camera.x;
		position.y -= camera.y;
	}
	
	for(int i = 0; auto const &elem : text)
	{
		// If the letter has a character on the map
		if(auto it = font.fontTable.find(elem);
		   it != font.fontTable.end())
		{
			using enum FontDrawNewLine;
			
			switch(newLine)
			{
				// If newLine == 1, we need to wait until a space to go to next line
				case 1:
					if(elem != std::string(" ")[0]) break;
					[[fallthrough]];
				// If newLine == 2, we need to go to next line now
				case 2:
					position.y += font.lineHeight + font.spacing.y;
					xAdvance = 0;
					newLine = 0;
					//In any of the two cases, if elem is an space we don't draw it
					if(elem == std::string(" ")[0]) continue;
					break;
				//If newLine == 0, we don't need to do anything
				default: [[likely]]
					break;
			}

			SDL_Rect rect = it->second.rect;
			iPoint tempPos(
				position.x + xAdvance + it->second.offset.x,
				position.y + static_cast<int>(static_cast<float>(it->second.offset.y) * scale.y));
			app->render->DrawFont(
				font.graphic,
				tempPos,
				scale,
				&rect,
				angle,
				pivot
			);
			
			xAdvance += static_cast<int>(static_cast<float>(it->second.xAdvance + font.spacing.x) * scale.x);

			if(maxX.first == SCREEN_FIT && xAdvance + position.x > camera.w - camera.x) [[unlikely]]
				newLine = 2;

			if(!newLine) [[unlikely]]
				newLine = CheckIfNewLine(position.x, xAdvance, maxX, i);
		}
		else LOG("Character %s could not be found in %s", elem, font.name.c_str());
	}
}

void Fonts::DrawMiddlePoint(std::string_view text, iPoint position, int fontId, fPoint scale, bool isFixed, std::pair<FontDrawNewLine, int> maxX, SDL_Point pivot, double angle) const
{
	if(!in_range(fontId, 0, static_cast<int>(fonts.size())))
	{
		LOG("%s: Invalid font id %d", __func__, fontId);
		return;
	}
	int total = 0;
	Font const &font = fonts[fontId];
	for(int i = 1; auto const &elem : text)
	{
		if(i == text.size() - 1) break;
		if(auto it = font.fontTable.find(elem);
		   it != font.fontTable.end())
		{
			total += it->second.rect.w + it->second.xAdvance + font.spacing.x;
		}
	}
	position.x -= static_cast<int>(scale.x * static_cast<float>(total) / 4.0f);
	position.y -= static_cast<int>(scale.y * static_cast<float>(font.lineHeight) / 2.0f);
	Draw(text, position, fontId, scale, isFixed, maxX, pivot, angle);
}

// WARNING: Modifies i. If you need to use it after calling this function, you need to do it before
int Fonts::CheckIfNewLine(int x, int xAdvance, std::pair<FontDrawNewLine, int> maxX, int &i) const
{
	auto const &[mode, max] = maxX;
	using enum FontDrawNewLine;
	
	if(mode == NONE || max == 0) return 0;
	
	if(mode == NUMBER_OF_CHARS) {
		if(max < i)
		{
			i = 0;
			return 1;
		}
		i++;
	}
	
	if(mode == POSITION_X_ON_MAP && max < xAdvance + x) return 1;
	
	if(mode == POSITION_X_ON_SCREEN && max < xAdvance) return 1;
	
	return 0;
}

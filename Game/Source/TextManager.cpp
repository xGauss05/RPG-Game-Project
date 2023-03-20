#include "App.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "Render.h"

#include <ranges>
#include <algorithm>
#include <string>
#include <iterator>

#include "Log.h"

void TextManager::Font::Unload() const
{
	app->tex->Unload(textureID);
}

// Constructor
TextManager::TextManager() : Module()
{
	name = "fonts";
}

// Destructor
TextManager::~TextManager()
{
	for (auto const& elem : fonts)
	{
		elem.Unload();
	}
}

bool TextManager::Awake(pugi::xml_node &config)
{
	path = config.child("property").attribute("path").as_string();
	return true;
}

bool TextManager::Start()
{
	app->fonts->Load("CleanCraters");
	return true;
}

// Loads the font named fontName with the folder path specified in config.xml.
// Returns:
// new font id, the font id if it was already loaded, or -1 if the texture or xml could not be loaded.
// WARNING: The font has to have a .xml file. If it doesn't, use the other Load function.
int TextManager::Load(std::string const &fontName)
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
		
		newFont.textureID = app->tex->Load(texturePath.c_str());
				
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
			Font::CharInfo newChar = {};
			
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
			
			newFont.charMap.try_emplace(
				elem.attribute("letter").as_string()[0],
				newChar
			);
		}

		// Add font into fonts vector
		if (freeVectorElements.empty()) [[likely]]
		{
			fonts.push_back(newFont);
		}
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

void TextManager::UnLoad(int font_id)
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

void TextManager::DrawText(std::string_view text, TextParameters const &textParams) const
{
	// Unwrap textparameters struct
	int fontId = textParams.fontId;
	DrawParameters params = textParams.params;

	if (fonts.empty() || !in_range(fontId, 0, static_cast<int>(fonts.size())))
	{
		LOG("%s: Invalid font id %d", __func__, fontId);
		return;
	}

	Font const &fontInUse = fonts[fontId];

	// Get starting drawing position
	params.position = GetAlignPosition(text, params.position, textParams.align, fontInUse);
	params.position = GetAnchorPosition(params.position, textParams.anchor);
	
	
	for (auto const& elem : text)
	{
		// Search for the specific character info in the character map
		auto charIter = fontInUse.charMap.find(elem);

		// If the character hasn't been found in the map
		if (charIter == fontInUse.charMap.end())
		{
			LOG("Character %s could not be found in %s", elem, fontInUse.name.c_str());
		}

		// Get the information of the character
		auto charInfo = charIter->second;

		// Set the rectangle that holds the character
		params.section = &charInfo.rect;

		// Offset position to align character
		params.position.x += charInfo.offset.x;
		params.position.y += charInfo.offset.y;

		// Draw the character on screen
		app->render->DrawTexture(params);
		
		// Update X position
		params.position.x += GetDistanceToNextDrawingPositon(
								charInfo.xAdvance,
								fontInUse.spacing.x,
								charInfo.offset.x
		);

		// Set Y to original position
		params.position.y -= charInfo.offset.y;
	}
}

inline iPoint TextManager::GetAnchorPosition(iPoint position, AnchorTo anchor) const
{
	using enum AnchorTo;
	switch (anchor)
	{
		case SCREEN:
			return position - iPoint(app->render->GetCamera().x, app->render->GetCamera().y);
		case WORLD:
			return position;
			break;
	}
}

iPoint TextManager::GetAlignPosition(std::string_view text, iPoint position, AlignTo align, Font const &font) const
{
	if (align == AlignTo::ALIGN_TOP_LEFT)
		return position;

	iPoint totalSize{ 0 };

	for (auto const& elem : text)
	{
		if (auto it = font.charMap.find(elem);
			it != font.charMap.end())
		{
			totalSize.x += it->second.rect.w + it->second.xAdvance + font.spacing.x;
		}
	}

	if (align == AlignTo::ALIGN_CENTER)
	{
		return iPoint(position.x - (totalSize.x / 4), position.y - (font.lineHeight / 2));
	}
	return position;
}

inline int TextManager::GetDistanceToNextDrawingPositon(int advance, int spacing, int offset, float scale) const
{
	auto distance = static_cast<float>(advance + spacing - offset);

	if (scale != 0.0f) distance *= scale;

	return static_cast<int>(std::floor(distance)) ;
}



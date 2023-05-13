#ifndef __GUIPANELSEGMENTED_H__
#define __GUIPANELSEGMENTED_H__

#include "Point.h"
#include "Defs.h"

#include "TextureManager.h"

enum class GuiPanel_Border
{
	NONE = 0x0000,
	LEFT = 0x0001,
	RIGHT = 0x0010,
	UP = 0x0100,
	DOWN = 0x1000,
	ALL = 0x1111
};

inline GuiPanel_Border operator&(GuiPanel_Border a, GuiPanel_Border b)
{
	return static_cast<GuiPanel_Border>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline bool AContainsB(GuiPanel_Border a, GuiPanel_Border b)
{
	return (a & b) == b;
}

class GuiPanelSegmented
{
public:
	GuiPanelSegmented() = delete;
	explicit GuiPanelSegmented(SDL_Rect const& r, iPoint a, int id, iPoint tSegments, GuiPanel_Border border = GuiPanel_Border::ALL);
	explicit GuiPanelSegmented(SDL_Rect const& r, int a, int id, iPoint tSegments, GuiPanel_Border border = GuiPanel_Border::ALL);

	void Draw(iPoint originalPosition, iPoint size) const;
	void Unload() const;

	int GetTextureID() const;

	GuiPanel_Border MapStringToGuiPanelBorder(std::string_view str) const;

private:
	void DrawHorizontalSegment(iPoint topLeftPosition, SDL_Rect currentSegment, iPoint size) const;

	SDL_Rect rect{ 0 };
	iPoint advance{ 0 };
	int textureID = -1;
	iPoint textureSegments = { 3, 3 };

	GuiPanel_Border drawBorder = GuiPanel_Border::ALL;

};

#endif // __GUIPANELSEGMENTED_H__

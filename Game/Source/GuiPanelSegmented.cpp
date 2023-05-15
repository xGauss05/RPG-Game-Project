#include "GuiPanelSegmented.h"
#include "App.h"

#include "Render.h"

GuiPanelSegmented::GuiPanelSegmented(SDL_Rect const& r, iPoint a, int id, iPoint tSegments, GuiPanel_Border border)
	: rect(r), advance(a), textureID(id), textureSegments(tSegments), drawBorder(border) {}

GuiPanelSegmented::GuiPanelSegmented(SDL_Rect const& r, int a, int id, iPoint tSegments, GuiPanel_Border border)
	: rect(r), advance(iPoint(a, a)), textureID(id), textureSegments(tSegments), drawBorder(border) {}

void GuiPanelSegmented::Draw(iPoint originalPosition, iPoint size) const
{
	SDL_Rect currentSegment = rect;
	currentSegment.x += advance.x;
	currentSegment.y += advance.y;

	iPoint currentPosition(originalPosition);

	auto yRepeats = size.y / currentSegment.h;

	for (int i = 0; i < yRepeats; i++)
	{
		DrawHorizontalSegment(currentPosition, currentSegment, size);

		// Go to next draw line
		currentPosition.y += currentSegment.h;

		// In first iteration we keep the Y coordinate of first side
		if (i == 0)
		{
			currentSegment.y += currentSegment.h + advance.y;
			originalPosition.x = currentSegment.y;
		}
		// In last iteration we go to the down corner
		else if (i == yRepeats - 2)
		{
			currentSegment.y = originalPosition.x + (textureSegments.y - 2) * (currentSegment.h + advance.y);
		}
		// In any other iteration, we iterate over the number of sides segments (different textures) there are
		else
		{
			if (i % (textureSegments.y - 2) != 0) currentSegment.y += currentSegment.h + advance.y;
			else currentSegment.y = originalPosition.x;
		}
	}
}

void GuiPanelSegmented::DrawHorizontalSegment(iPoint topLeftPosition, SDL_Rect currentSegment, iPoint size) const
{
	using enum GuiPanel_Border;

	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };
	topLeftPosition -= camera;

	if (!AContainsB(drawBorder, LEFT))
	{
		currentSegment.x += currentSegment.w + advance.x;
	}

	// Draw left border texture
	app->render->DrawTexture(DrawParameters(textureID, topLeftPosition).Section(&currentSegment));

	// Go to next draw position
	topLeftPosition.x += currentSegment.w;

	// Get number of side fragments (minus corners) needed to fill the side
	auto xRepeats = (size.x / currentSegment.w) - 2;

	
	if(AContainsB(drawBorder, LEFT))
		currentSegment.x += currentSegment.w + advance.x;

	auto firstSideX = currentSegment.x;

	// Draw sides
	for (int i = 0; i < xRepeats; i++)
	{
		app->render->DrawTexture(DrawParameters(textureID, topLeftPosition).Section(&currentSegment));

		topLeftPosition.x += currentSegment.w;

		if (textureSegments.x > 2)
		{
				// Go to next side fragment, if there are no more, go to previous
				currentSegment.x = (i % (textureSegments.x - 2))
				? currentSegment.x + currentSegment.w + advance.x
				: firstSideX;
		}
	}

	if(textureSegments.x - 2 > 0)
		currentSegment.x = firstSideX + (textureSegments.x - 2) * (currentSegment.w + advance.x);
	else if (AContainsB(drawBorder, RIGHT))
		currentSegment.x += currentSegment.w + advance.x;		

	// Draw right corner
	app->render->DrawTexture(DrawParameters(textureID, topLeftPosition).Section(&currentSegment));
}

void GuiPanelSegmented::Unload() const
{
	app->tex->Unload(textureID);	//I don't know if removing this line will cause memory leaks (probably), but if you pop a
									//Window_List out of a vector the texture is deleted from the scene and nothing else can use it.
}

int GuiPanelSegmented::GetTextureID() const
{
	return textureID;
}

GuiPanel_Border GuiPanelSegmented::MapStringToGuiPanelBorder(std::string_view str) const
{
	using enum GuiPanel_Border;

	if (StrEquals(str, "NONE"))		return NONE;
	if (StrEquals(str, "LEFT"))		return LEFT;
	if (StrEquals(str, "RIGHT"))	return RIGHT;
	if (StrEquals(str, "UP"))		return UP;
	if (StrEquals(str, "DOWN"))		return DOWN;
	
	return ALL;
}

iPoint GuiPanelSegmented::GetSegmentCount() const
{
	return textureSegments;
}

SDL_Rect GuiPanelSegmented::GetSegmentSize() const
{
	return rect;
}

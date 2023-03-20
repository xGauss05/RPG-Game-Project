#include "GuiPanelSegmented.h"
#include "App.h"

#include "Render.h"

GuiPanelSegmented::GuiPanelSegmented(SDL_Rect const& r, iPoint a, int id, iPoint tSegments)
	: rect(r), advance(a), textureID(id), textureSegments(tSegments) {}

GuiPanelSegmented::GuiPanelSegmented(SDL_Rect const& r, int a, int id, iPoint tSegments)
	: rect(r), advance(iPoint(a, a)), textureID(id), textureSegments(tSegments) {}

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
	// Draw left corner texture
	app->render->DrawTexture(DrawParameters(textureID, topLeftPosition).Section(&currentSegment));

	// Go to next draw position
	topLeftPosition.x += currentSegment.w;

	// Get number of side fragments (minus corners) needed to fill the side
	auto xRepeats = (size.x / currentSegment.w) - 2;

	currentSegment.x += currentSegment.w + advance.x;

	auto firstSideX = currentSegment.x;

	// Draw sides
	for (int i = 0; i < xRepeats; i++)
	{
		app->render->DrawTexture(DrawParameters(textureID, topLeftPosition).Section(&currentSegment));

		topLeftPosition.x += currentSegment.w;

		// Go to next side fragment, if there are no more, go to previous
		currentSegment.x = (i % (textureSegments.x - 2))
			? currentSegment.x + currentSegment.w + advance.x
			: firstSideX;
	}

	currentSegment.x = firstSideX + (textureSegments.x - 2) * (currentSegment.w + advance.x);

	// Draw right corner
	app->render->DrawTexture(DrawParameters(textureID, topLeftPosition).Section(&currentSegment));
}

void GuiPanelSegmented::Unload() const
{
	app->tex->Unload(textureID);
}

#ifndef __GUISEGMENTEDPANEL_H__
#define __GUISEGMENTEDPANEL_H__

#include "TextureManager.h"

#include "Point.h"

#include <deque>
#include <chrono>

class GuiSegmentedPanel
{
public:
	GuiSegmentedPanel() = default;
	virtual ~GuiSegmentedPanel();

	void SetPanelArea(SDL_Rect const& destination);
	virtual void Draw() const;

	void UpdateAnimations();

protected:
	std::chrono::time_point<std::chrono::steady_clock> arrowAnimTimer;

	int textureID = -1;
	SDL_Rect dstRect = { 0, 0, 0, 0 };
	int currentArrowFrame = 0;

	SDL_Color windowColor = {255, 255, 255, 78 };

	const int segmentSize = 8;
	const int numberOfSegments = 3;
	const int arrowFrames = 4;

	const SDL_Rect border =
	{ 
		0, 0,
		segmentSize * numberOfSegments,
		segmentSize * numberOfSegments
	};

	const SDL_Rect background =
	{
		segmentSize * numberOfSegments,
		0,
		segmentSize * numberOfSegments,
		segmentSize * numberOfSegments
	};
	const SDL_Rect arrow =
	{
		0,
		segmentSize * numberOfSegments,
		segmentSize * numberOfSegments,
		segmentSize * numberOfSegments
	};
};

#endif // __GUIPANELSEGMENTED_H__

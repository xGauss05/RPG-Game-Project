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

	virtual void SetPanelArea(SDL_Rect const& destination);
	virtual void Draw() const;
	void DrawArrow() const;
	void DrawTopArrow() const;
	void UpdateAnimations();

	void SetActive(bool b);
	bool IsActive() const;

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

protected:
	std::chrono::time_point<std::chrono::steady_clock> arrowAnimTimer;

	int textureID = -1;
	SDL_Rect dstRect = { 0, 0, 0, 0 };
	int currentArrowFrame = 0;

	SDL_Color windowColor = {255, 255, 255, 78 };

	bool isActive = false;
};

#endif // __GUIPANELSEGMENTED_H__

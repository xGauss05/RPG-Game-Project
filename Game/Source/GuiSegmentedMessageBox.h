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
	~GuiSegmentedPanel();

	void SetMessageArea(SDL_Rect const& destination);
	void Draw() const;

	void AddMessageToQueue(std::string_view message);

	// Returns empty string if there are no messages
	std::string_view GetNextMessage() const;

	// Removes front message in queue
	// Returns true if there are more messages pending
	bool RemoveCurrentMessage();

	void ReplaceCurrentMessage(std::string_view str);

	bool IsInputLocked() const;
	void LockInput();

	void UpdateAnimations();

private:
	std::chrono::time_point<std::chrono::steady_clock> arrowAnimTimer;

	std::deque<std::string> messageQueue;

	bool lockInput = false;

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

#ifndef __GUISEGMENTEDMESSAGEBOX_H__
#define __GUISEGMENTEDMESSAGEBOX_H__

#include "GuiSegmentedPanel.h"

#include "Point.h"

#include <deque>
#include <chrono>

class GuiSegmentedMessageBox : public GuiSegmentedPanel
{
public:

	void Draw() const override;

	void AddMessageToQueue(std::string_view message);

	// Returns empty string if there are no messages
	std::string_view GetNextMessage() const;

	// Removes front message in queue
	// Returns true if there are more messages pending
	bool RemoveCurrentMessage();

	void ReplaceCurrentMessage(std::string_view str);

	bool IsInputLocked() const;
	void LockInput();

private:
	std::deque<std::string> messageQueue;
	bool lockInput = false;

};

#endif // __GUISEGMENTEDMESSAGEBOX_H__

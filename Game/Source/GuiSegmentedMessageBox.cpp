#include "GuiSegmentedMessageBox.h"
#include "App.h"

#include "Render.h"

#include "TextManager.h"

void GuiSegmentedMessageBox::Draw() const
{
	GuiSegmentedPanel::Draw();

	if (SDL_RectEmpty(&dstRect))
	{
		return;
	}

	SDL_Rect camera = app->render->GetCamera();
	iPoint drawPosition = { dstRect.x - camera.x, dstRect.y - camera.y };
	
	if (!messageQueue.empty())
	{
		app->fonts->DrawText(
			messageQueue.front(),
			true,
			TextParameters(
				0,
				DrawParameters(0, iPoint(dstRect.x, dstRect.y))
					.Section(&dstRect)
			)
		);
	}

	if (lockInput || messageQueue.size() > 1)
	{
		drawPosition =
		{
			((dstRect.x + dstRect.w) / 2) - (arrow.w / 2),
			dstRect.y + dstRect.h - arrow.y
		};

		SDL_Rect srcRect = arrow;
		srcRect.x = (segmentSize * numberOfSegments) * (currentArrowFrame % 4);

		app->render->DrawTexture(DrawParameters(textureID, drawPosition).Section(&srcRect));
	}
}

void GuiSegmentedMessageBox::AddMessageToQueue(std::string_view message)
{
	if(!message.empty())
		messageQueue.emplace_back(message);
}

std::string_view GuiSegmentedMessageBox::GetNextMessage() const
{
	if (messageQueue.empty())
	{
		return std::string_view();
	}

	return messageQueue.front();
}

bool GuiSegmentedMessageBox::RemoveCurrentMessage()
{
	if (!messageQueue.empty())
	{
		messageQueue.pop_front();
	}

	if (messageQueue.empty())
	{
		lockInput = false;
		return false;
	}

	return true;
}

void GuiSegmentedMessageBox::ReplaceCurrentMessage(std::string_view str)
{
	if (!messageQueue.empty())
	{
		messageQueue.pop_front();
	}

	if (!str.empty())
		messageQueue.emplace_front(str);
}

bool GuiSegmentedMessageBox::IsInputLocked() const
{
	return lockInput;
}

void GuiSegmentedMessageBox::LockInput()
{
	lockInput = true;
}

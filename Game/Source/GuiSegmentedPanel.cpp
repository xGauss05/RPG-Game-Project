#include "GuiSegmentedPanel.h"
#include "App.h"

#include "Render.h"

#include "TextManager.h"

GuiSegmentedPanel::~GuiSegmentedPanel()
{
	app->tex->Unload(textureID);
}

void GuiSegmentedPanel::SetMessageArea(SDL_Rect const& destination)
{
	textureID = app->tex->Load("Assets/UI/WindowSkins.png");
	dstRect = destination;
	arrowAnimTimer = std::chrono::steady_clock::now();
	SDL_SetTextureColorMod(app->GetTexture(textureID), windowColor.r, windowColor.g, windowColor.b);
}

void GuiSegmentedPanel::Draw() const
{
	if (SDL_RectEmpty(&dstRect))
	{
		return;
	}

	SDL_Rect camera = app->render->GetCamera();
	iPoint drawPosition = { dstRect.x - camera.x, dstRect.y - camera.y };
	DrawParameters params = { textureID, drawPosition };
	SDL_Rect bgSegment =
	{
		background.x,
		background.y,
		segmentSize,
		segmentSize
	};
	SDL_Rect borderSegment = 
	{
		border.x,
		border.y,
		segmentSize,
		segmentSize
	};

	iPoint repeats =
	{
		dstRect.w / segmentSize,
		dstRect.h / segmentSize
	};

	iPoint orphanPixels =
	{
		 dstRect.w - (repeats.x * segmentSize),
		 dstRect.h - (repeats.y * segmentSize)
	};

	for (int j = 0; j < repeats.y; j++)
	{
		if (j == 1 || j == repeats.y - 1)
		{
			bgSegment.y += segmentSize;
			borderSegment.y += segmentSize;
		}

		app->render->DrawTexture(params.Section(&bgSegment));
		app->render->DrawTexture(params.Section(&borderSegment));

		params.position.x += segmentSize;
		bgSegment.x += segmentSize;
		borderSegment.x += segmentSize;

		for (int i = 0; i < repeats.x; i++)
		{
			app->render->DrawTexture(params.Section(&bgSegment));
			app->render->DrawTexture(params.Section(&borderSegment));
			params.position.x += segmentSize;
		}

		bgSegment.x += segmentSize - 1;
		borderSegment.x += segmentSize - 1;

		for (int i = 0; i < orphanPixels.x; i++)
		{
			bgSegment.w = 1;
			borderSegment.w = 1;
			app->render->DrawTexture(params.Section(&bgSegment));
			app->render->DrawTexture(params.Section(&borderSegment));
			bgSegment.w = segmentSize;
			borderSegment.w = segmentSize;
			params.position.x += 1;
		}

		bgSegment.x += 1;
		borderSegment.x += 1;

		app->render->DrawTexture(params.Section(&bgSegment));
		app->render->DrawTexture(params.Section(&borderSegment));

		borderSegment.x = border.x;
		bgSegment.x = background.x;
		params.position.x = dstRect.x - camera.x;
		params.position.y += segmentSize;
	}


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

	if (lockInput)
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

void GuiSegmentedPanel::AddMessageToQueue(std::string_view message)
{
	if(!message.empty())
		messageQueue.emplace_back(message);
}

std::string_view GuiSegmentedPanel::GetNextMessage() const
{
	if (messageQueue.empty())
	{
		return std::string_view();
	}

	return messageQueue.front();
}

bool GuiSegmentedPanel::RemoveCurrentMessage()
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

void GuiSegmentedPanel::ReplaceCurrentMessage(std::string_view str)
{
	if (!messageQueue.empty())
	{
		messageQueue.pop_front();
	}

	if (!str.empty())
		messageQueue.emplace_front(str);
}

bool GuiSegmentedPanel::IsInputLocked() const
{
	return lockInput;
}

void GuiSegmentedPanel::LockInput()
{
	lockInput = true;
}

void GuiSegmentedPanel::UpdateAnimations()
{
	if (lockInput)
	{
		auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - arrowAnimTimer);
		if (timeElapsed.count() >= 200)
		{
			arrowAnimTimer = std::chrono::steady_clock::now();
			currentArrowFrame++;
		}
	}
}


#include "GuiMenuList.h"
#include "App.h"
#include "Render.h"
#include "TextManager.h"
#include "Log.h"

GuiMenuList::GuiMenuList(pugi::xml_node const& node) :
	position(iPoint(node.attribute("x").as_int(), node.attribute("y").as_int())),
	size(iPoint(node.attribute("width").as_int(), node.attribute("height").as_int()))
{
	if (auto backgroundNode = node.child("background");
		backgroundNode)
	{
		SDL_Rect rect(
			backgroundNode.attribute("segmentX").as_int(),
			backgroundNode.attribute("segmentY").as_int(),
			backgroundNode.attribute("segmentWidth").as_int(),
			backgroundNode.attribute("segmentHeight").as_int()
		);

		int advance = backgroundNode.attribute("advance").as_int();

		int textureID = app->tex->Load(backgroundNode.attribute("path").as_string());

		iPoint tSegments(0, 0);

		if (auto boxSegments = backgroundNode.attribute("segments").as_int();
			boxSegments != 0)
		{
			tSegments = { boxSegments, boxSegments };
		}
		else
		{
			tSegments = {
				backgroundNode.attribute("numOfSegmentsX").as_int(),
				backgroundNode.attribute("numOfSegmentsY").as_int()
			};
		}

		GuiPanel_Border missingPanel = GuiPanel_Border::NONE;

		// Something something about accessing nullpointers?
		// How in the hell does this work and why
		for (auto const& elem : backgroundNode.children("border"))
		{
			missingPanel = static_cast<GuiPanel_Border>(
				static_cast<uint>(missingPanel) +
				static_cast<uint>(
					background->MapStringToGuiPanelBorder(
						elem.attribute("missing").as_string()
					)
				)
			);
		}

		auto hasPanels =
			static_cast<GuiPanel_Border>(
				static_cast<uint>(GuiPanel_Border::ALL) - static_cast<uint>(missingPanel)
			);

		background = std::make_unique<GuiPanelSegmented>(rect, advance, textureID, tSegments, hasPanels);

		if (auto scrollArrowNode = backgroundNode.child("scrollarrow");
			scrollArrowNode)
		{
			arrowRect = {
				scrollArrowNode.attribute("rectX").as_int(),
				scrollArrowNode.attribute("rectY").as_int(),
				scrollArrowNode.attribute("rectW").as_int(),
				scrollArrowNode.attribute("rectH").as_int()
			};
		}
	}

	if (auto itemNode = node.child("itemlist");
		itemNode)
	{
		iconSize = itemNode.attribute("iconSize").as_int();
		fontID = itemNode.attribute("font").as_int();
		maxColumns = itemNode.attribute("maxColumns").as_int();

		itemMargin = iPoint(itemNode.attribute("itemMarginX").as_int(), itemNode.attribute("itemMarginY").as_int());
		outterMargin = iPoint(itemNode.attribute("outterMarginX").as_int(), itemNode.attribute("outterMarginY").as_int());

		menuItemHeight = app->fonts->GetLineHeight(fontID) + (itemMargin.y * 2);

		maxElements = itemNode.attribute("maxElements").as_int();

		if (maxElements == -1)
		{
			maxElements = size.y / menuItemHeight;
		}

		outterMargin.y += (arrowRect.h / 2);

		int correctedYSize = outterMargin.y * 2 + (menuItemHeight * maxElements);

		size.y = correctedYSize;

		size.CeilToNearest(32);
	}

	for (int i = 0; i < 20; i++)
	{
		items.emplace_back(MenuItem::ItemText("Hi", "By", std::format("x{}", i)), 9);
	}
}

GuiMenuList::~GuiMenuList()
{
	background->Unload();
}

void GuiMenuList::Update()
{
	HandleInput();
}

bool GuiMenuList::Draw() const
{
	background->Draw(position, size);

	if (!SDL_RectEmpty(&arrowRect))
	{
		iPoint arrowPos =
		{
			position.x + (size.x - arrowRect.w)/ 2,
			position.y
		};
		if (currentScroll > 0)
		{
			app->render->DrawTexture(
				DrawParameters(background->GetTextureID(), arrowPos)
				.Section(&arrowRect)
				.Flip(SDL_RendererFlip::SDL_FLIP_VERTICAL)
			);
		}
		if (currentScroll + maxElements < items.size())
		{
			arrowPos.y += size.y - arrowRect.h;

			app->render->DrawTexture(
				DrawParameters(background->GetTextureID(), arrowPos)
				.Section(&arrowRect)
			);
		}
	}

	for (int i = currentScroll; i < currentScroll + maxElements && i < items.size(); ++i)
	{
		iPoint drawPosition(
			position.x + outterMargin.x,
			position.y + outterMargin.y + (menuItemHeight * (i - currentScroll))
		);

		items[i].Draw(drawPosition, iPoint(size.x, menuItemHeight), itemMargin, iconSize);

		//items[i].DebugDraw(drawPosition, iPoint(size.x, menuItemHeight), outterMargin.y, menuItemHeight, i, currentScroll);
	}

	return true;
}

void GuiMenuList::DebugDraw() const
{
	SDL_Rect debugRect(position.x, size.y, size.x, size.y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

void GuiMenuList::HandleLeftButtonClick(int result)
{
}

void GuiMenuList::HandleRightButtonClick()
{
}

void GuiMenuList::HandleInput()
{
	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN
		|| app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
	{
		HandleLeftClick();
	}
	else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
	{
		HandleRightButtonClick();
	}

	lastTimeSinceScrolled++;

	if (lastTimeSinceScrolled >= 10)
		HandleWheelScroll();
}

void GuiMenuList::HandleLeftClick()
{
	iPoint mousePos = app->input->GetMousePosition();

	auto IsMouseHovering = [mousePos](iPoint currentPos, iPoint totalSize)
	{
		return (mousePos.x >= currentPos.x && mousePos.x <= currentPos.x + totalSize.x &&
				mousePos.y >= currentPos.y && mousePos.y <= currentPos.y + totalSize.y);
	};

	if (IsMouseHovering(position, size))
	{
		iPoint relativeCoords = mousePos - position - outterMargin;

		if (relativeCoords.y <= 0 && lastTimeSinceScrolled >= 15)
		{
			SelectAndScrollUpIfNeeded();
			return;
		}

		int elementClicked = currentScroll + relativeCoords.y / menuItemHeight;

		int elementInRange = MIN(
			elementClicked,
			currentScroll + maxElements - 1,		// Max index of item that are shown in screen
			{ static_cast<int>(items.size()) }	// Max number of available items
		);

		if (elementClicked == elementInRange)
		{
			if(app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN
				&& currentItemSelected == elementClicked)
			{
				HandleLeftButtonClick(elementClicked);
			}
			else
			{
				currentItemSelected = elementClicked;
			}
		}
		else if (lastTimeSinceScrolled >= 15)
		{
			SelectAndScrollDownIfNeeded();
		}
	}
}

void GuiMenuList::HandleWheelScroll()
{
	int verticalWheelMotion = app->input->GetYWheelMotion();

	verticalWheelMotion *= -1;

	if (verticalWheelMotion < 0)
	{
		ScrollListUp();
	}
	else if (verticalWheelMotion > 0)
	{
		ScrollListDown();
	}
}

void GuiMenuList::SelectAndScrollUpIfNeeded(int amount)
{
	if (currentItemSelected > amount)
	{
		currentItemSelected -= amount;
	}
	else
	{
		currentItemSelected = 0;
	}

	if (currentItemSelected < currentScroll)
	{
		ScrollListUp();
	}
}

void GuiMenuList::SelectAndScrollDownIfNeeded(int amount)
{
	if (currentItemSelected < items.size() - amount)
	{
		currentItemSelected += amount;
	}
	else
	{
		currentItemSelected = items.size() - 1;
	}

	if (currentItemSelected >= currentScroll + maxElements)
	{
		ScrollListDown(amount);
	}
}

void GuiMenuList::ScrollListUp(int amount)
{
	// If it can still scroll up
	if(currentScroll > 0)
	{
		lastTimeSinceScrolled = 0;

		if (currentScroll - amount < 0)
			currentScroll = 0;
		else
			currentScroll -= amount;
	}
}

void GuiMenuList::ScrollListDown(int amount)
{
	// If it can still scroll down
	if (currentScroll + maxElements < items.size())
	{
		lastTimeSinceScrolled = 0;
		
		if (currentScroll + amount >= maxElements)
			currentScroll = items.size() - maxElements;
		else
			currentScroll += amount;
	}
}

GuiMenuList::MenuItem::MenuItem(ItemText const& itemText, int textureID)
	: text(itemText), iconTexture(textureID)
{}

void GuiMenuList::MenuItem::Draw(iPoint originalPos, iPoint rectSize, iPoint innerMargin, int sizeIcon) const
{
	iPoint drawPosition = originalPos + innerMargin;

	if (sizeIcon > 0)
	{
		if(iconTexture != -1)
		{
			drawPosition.y += 5;
			app->render->DrawTexture(DrawParameters(iconTexture, drawPosition));
			drawPosition.y -= 5;
		}

		drawPosition.x += sizeIcon + innerMargin.x;
	}

	if (!text.leftText.empty())
	{
		app->fonts->DrawText(
			text.leftText,
			TextParameters(
				0,
				DrawParameters(0, drawPosition)
			).Align(AlignTo::ALIGN_TOP_LEFT)
		);
	}

	if (!text.centerText.empty())
	{
		// Center the draw position
		drawPosition.x = originalPos.x + (rectSize.x / 2);
		drawPosition.y = originalPos.y + (rectSize.y / 2);

		app->fonts->DrawText(
			text.centerText,
			TextParameters(
				0,
				DrawParameters(0, drawPosition)
			).Align(AlignTo::ALIGN_CENTER)
		);
	}

	if (!text.rightText.empty())
	{
		drawPosition.x = originalPos.x + rectSize.x - innerMargin.x;
		drawPosition.x -= 32 - (drawPosition.x % 32);

		drawPosition.y = originalPos.y + innerMargin.y;
		
		app->fonts->DrawText(
			text.rightText,
			TextParameters(
				0,
				DrawParameters(0, drawPosition)
			).Align(AlignTo::ALIGN_TOP_RIGHT)
		);
	}
}

void GuiMenuList::MenuItem::DebugDraw(iPoint pos, iPoint s, int outterMarginY, int menuItemHeight, int index, int scroll) const
{
	iPoint debugDrawPos(
			pos.x,
			pos.y + outterMarginY + (menuItemHeight * (index - scroll))
	);

	SDL_Rect debugRect(pos.x, pos.y, s.x, s.y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}


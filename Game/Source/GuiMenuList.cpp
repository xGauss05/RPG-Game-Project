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

		iPoint tSegments(
			backgroundNode.attribute("segments").as_int(),
			backgroundNode.attribute("segments").as_int()
		);

		background = std::make_unique<GuiPanelSegmented>(rect, advance, textureID, tSegments);
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

		int correctedYSize = outterMargin.y * 2 + (menuItemHeight * maxElements);

		size.y = correctedYSize;

		size.CeilToNearest(32);
	}

	for (int i = 0; i < 10; i++)
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
	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
	{
		iPoint mousePos = app->input->GetMousePosition();

		auto IsMouseHovering = [mousePos](iPoint p, iPoint s)
		{
			return (mousePos.x >= p.x && mousePos.x <= p.x + s.x &&
					mousePos.y >= p.y && mousePos.y <= p.y + s.y);
		};

		if (IsMouseHovering(position, size))
		{
			iPoint relativeCoords = mousePos - position + itemMargin;
			int elementClicked = relativeCoords.y / menuItemHeight;
			
			if(elementClicked < maxElements)
				HandleUserClick(elementClicked);
		}
	}
}

bool GuiMenuList::Draw() const
{
	background->Draw(position, size);

	for (int i = currentScroll; i < maxElements && i < items.size(); ++i)
	{
		iPoint drawPosition(
			position.x + outterMargin.x,
			position.y + outterMargin.y + (menuItemHeight * (i - currentScroll))
		);

		items[i].Draw(drawPosition, iPoint(size.x, menuItemHeight), itemMargin, iconSize);
	}

	return true;
}

void GuiMenuList::DebugDraw() const
{
	SDL_Rect debugRect(position.x, size.y, size.x, size.y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
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
			app->render->DrawTexture(DrawParameters(iconTexture, drawPosition));

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
		LOG("%i, %i", app->input->GetMousePosition().x, app->input->GetMousePosition().y);
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


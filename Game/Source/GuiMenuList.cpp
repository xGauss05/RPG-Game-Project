#include "GuiMenuList.h"
#include "App.h"
#include "Render.h"
#include "TextManager.h"
#include "Log.h"
#include "GameParty.h"

GuiMenuList::GuiMenuList() = default;

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

		if (iconSize > menuItemHeight)
		{
			menuItemHeight = iconSize + (itemMargin.y * 5);
		}

		maxElements = itemNode.attribute("maxElements").as_int();

		if (maxElements == -1)
		{
			maxElements = size.y / menuItemHeight;
		}

		outterMargin.y += (arrowRect.h / 2);

		int correctedYSize = outterMargin.y * 2 + (menuItemHeight * maxElements);

		size.y = correctedYSize;

		if (background)
		{
			size.CeilToNearest(background->GetSegmentSize().h);
		}
	}
}

GuiMenuList::~GuiMenuList()
{
	if(background)
		background->Unload();
}

void GuiMenuList::Initialize()
{
	InitializeElements();
	SetDefaultBooleanValues();
}

void GuiMenuList::Start()
{
	if (!items.empty() || !characters.empty())
		SetCurrentItemSelected(0);
	else
		SetCurrentItemSelected(-1);

	SetDefaultBooleanValues();
}

void GuiMenuList::SetDefaultBooleanValues()
{
	deleteMenu = false;
	goToPreviousMenu = false;
	clickHandled = false;
	alphaDirection = 1;
}

bool GuiMenuList::Update()
{
	HandleInput();
	UpdateAlpha();

	if (clickHandled || closeAllMenus)
	{
		clickHandled = false;
		return true;
	}

	return false;
}

void GuiMenuList::UpdateAlpha()
{
	currentAlpha += (5 * alphaDirection);
	if (currentAlpha <= g_guiItemMinAlpha || currentAlpha >= g_guiItemMaxAlpha)
	{
		currentAlpha = std::ranges::clamp(currentAlpha, g_guiItemMinAlpha, g_guiItemMaxAlpha);
		alphaDirection *= -1;
	}
}

void GuiMenuList::CreateMenuItem(MenuItem const& item)
{
	items.push_back(item);
}

void GuiMenuList::DeleteMenuItem(int index)
{
	items.erase(items.begin() + index);
}

bool GuiMenuList::Draw() const
{
	if(background)
		background->Draw(position, size);


	if(!items.empty())
	{
		if (!SDL_RectEmpty(&arrowRect))
		{
			iPoint arrowPos =
			{
				position.x + (size.x - arrowRect.w) / 2,
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

			items[i].Draw(drawPosition, iPoint(size.x, menuItemHeight), itemMargin, outterMargin, currentAlpha, iconSize, (currentItemSelected == i));
		}
	}
	else if (!characters.empty())
	{
		for (int i = currentScroll; i < currentScroll + maxElements && i < characters.size(); ++i)
		{
			iPoint drawPosition(
				position.x + outterMargin.x,
				position.y + outterMargin.y + (menuItemHeight * (i - currentScroll))
			);

			characters[i].Draw(drawPosition, iPoint(size.x, menuItemHeight), itemMargin, outterMargin, currentAlpha, iconSize, (currentItemSelected == i));
		}
	}

	return true;
}

void GuiMenuList::DebugDraw() const
{
	SDL_Rect debugRect(position.x, size.y, size.x, size.y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

void GuiMenuList::SetClickHandled(bool b)
{
	clickHandled = b;
}

void GuiMenuList::SetDeleteMenu(bool b)
{
	deleteMenu = b;
}

bool GuiMenuList::GoToPreviousMenu() const
{
	return deleteMenu;
}

bool GuiMenuList::GetClickHandled() const
{
	return clickHandled;
}

int GuiMenuList::GetLastClick() const
{
	return lastClick;
}

void GuiMenuList::SetPreviousPanelLastClick(int n)
{
	previousPanelLastClick = n;
}

int GuiMenuList::GetPreviousPanelLastClick() const
{
	return previousPanelLastClick;
}

void GuiMenuList::CreateMenuCharacter(Battler const& battler)
{
	characters.emplace_back(battler);
}

void GuiMenuList::SetGoToPreviousMenu(bool b)
{
	goToPreviousMenu = b;
}

void GuiMenuList::SetCurrentAlpha(uint8_t value)
{
	currentAlpha = value;
}

void GuiMenuList::SetCurrentItemSelected(int value)
{
	currentItemSelected = value;
}

void GuiMenuList::ResetCurrentItemSelected()
{
	currentItemSelected = -1;
}

void GuiMenuList::ClearMenuItems()
{
	items.clear();
	characters.clear();
}

bool GuiMenuList::GetGoToPreviousMenu() const
{
	return goToPreviousMenu;
}

bool GuiMenuList::GetAndDefaultCloseAllMenus()
{
	bool ret = closeAllMenus;
	closeAllMenus = false;
	return ret;
}

void GuiMenuList::SetActive(bool n)
{
	active = n;
	if (active)
	{
		SetDefaultBooleanValues();
	}
}

void GuiMenuList::ToggleActive()
{
	active = !active;
}

bool GuiMenuList::IsActive() const
{
	return active;
}

std::size_t GuiMenuList::GetNumberOfItems() const
{
	return items.size();
}

void GuiMenuList::HandleInput()
{
	using enum KeyState;
	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		HandleLeftClick();
	}
	else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		HandleRightButtonClick();
	}
	else if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		closeAllMenus = true;
	}

	lastTimeSinceScrolled++;

	if (lastTimeSinceScrolled >= 10)
		HandleWheelScroll();
}

void GuiMenuList::HandleLeftClick()
{
	if (items.empty() && characters.empty())
		return;

	iPoint mousePos = app->input->GetMousePosition();

	auto IsMouseHovering = [mousePos](iPoint currentPos, iPoint totalSize)
	{
		return (mousePos.x >= currentPos.x && mousePos.x <= currentPos.x + totalSize.x &&
				mousePos.y >= currentPos.y && mousePos.y <= currentPos.y + totalSize.y);
	};

	if (IsMouseHovering(position, size))
	{
		iPoint relativeCoords = mousePos - position - outterMargin;

		if (relativeCoords.y <= 0)
		{
			if(lastTimeSinceScrolled >= 15)
				SelectAndScrollUpIfNeeded();

			return;
		}

		int elementClicked = currentScroll + relativeCoords.y / menuItemHeight;

		int vectorToLookInto = items.empty() ? characters.size() : items.size();

		int elementInRange = MIN(
			elementClicked,
			currentScroll + maxElements - 1,		// Max index of item that are shown in screen
			{ vectorToLookInto }	// Max number of available items
		);

		if (elementClicked == elementInRange)
		{
			if(app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN
				&& currentItemSelected == elementClicked)
			{
				SetLastClick(elementClicked);
				currentAlpha = 195;
				alphaDirection = -1;
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
		lastTimeSinceScrolled = 0;
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
	int vectorSize = items.empty() ? characters.size() : items.size();
	if (currentItemSelected < vectorSize - amount)
	{
		lastTimeSinceScrolled = 0;
		currentItemSelected += amount;
	}
	else
	{
		currentItemSelected = vectorSize - 1;
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
	int vectorSize = items.empty() ? characters.size() : items.size();
	// If it can still scroll down
	if (currentScroll + maxElements < vectorSize)
	{
		lastTimeSinceScrolled = 0;


		if (currentScroll + amount >= maxElements)
			currentScroll = vectorSize - maxElements;
		else
			currentScroll += amount;
	}
}

void GuiMenuList::SetLastClick(int i)
{
	lastClick = i;
}

GuiMenuList::MenuItem::MenuItem(ItemText const& itemText, int textureID)
	: text(itemText), iconTexture(textureID)
{}

void GuiMenuList::MenuItem::Draw(iPoint originalPos, iPoint rectSize, iPoint innerMargin, iPoint outMargin, Uint8 animationAlpha, int sizeIcon, bool currentlySelected) const
{
	iPoint drawPosition = originalPos + innerMargin;

	if (currentlySelected)
	{
		iPoint cam = { app->render->GetCamera().x, app->render->GetCamera().y };
		SDL_Rect selectedRect = {
			(-1 * cam.x) + originalPos.x,
			(-1 * cam.y) + originalPos.y,
			rectSize.x - (outMargin.x * 2), 
			rectSize.y - innerMargin.y
		};
		app->render->DrawShape(selectedRect, true, SDL_Color(255, 255, 255, animationAlpha));
	}

	if (sizeIcon > 0)
	{
		if (iconTexture != -1)
		{
			iPoint cam ={ -1 * app->render->GetCamera().x, -1 * app->render->GetCamera().y };
			
			drawPosition += cam;
			drawPosition.y += 2;
			app->render->DrawTexture(DrawParameters(iconTexture, drawPosition));
			drawPosition.y -= 2;
			drawPosition -= cam;
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

void GuiMenuList::MenuItem::DebugDraw(iPoint pos, iPoint s, int outterMarginY, int itemHeight, int index, int scroll) const
{
	iPoint debugDrawPos(
			pos.x,
			pos.y + outterMarginY + (itemHeight * (index - scroll))
	);

	SDL_Rect debugRect(pos.x, pos.y, s.x, s.y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

void GuiMenuList::MenuItem::SetText(ItemText const& newText)
{
	text = newText;
}

void GuiMenuList::MenuItem::SetText(int align, std::string_view newText)
{
	switch (align)
	{
	case 1:
		text.leftText = newText;
		return;
	case 2:
		text.centerText = newText;
		return;
	case 3:
		text.rightText = newText;
		return;
	default:
		break;
	}
}


GuiMenuList::MenuCharacter::MenuCharacter(Battler const& battler)
	: character(battler)
{
	if(hpBarTexture == -1)
		hpBarTexture = app->tex->Load("Assets/UI/HP_Bar.png");
}

GuiMenuList::MenuCharacter::~MenuCharacter()
{
	app->tex->Unload(hpBarTexture);
}

void GuiMenuList::MenuCharacter::Draw(iPoint originalPos, iPoint rectSize, iPoint innerMargin, iPoint outMargin, Uint8 animationAlpha, int iconSize, bool currentlySelected) const
{
	iPoint drawPosition = originalPos + innerMargin;

	if (currentlySelected)
	{
		iPoint cam = { app->render->GetCamera().x, app->render->GetCamera().y };
		SDL_Rect selectedRect = {
			(-1 * cam.x) + originalPos.x,
			(-1 * cam.y) + originalPos.y,
			rectSize.x - (outMargin.x * 2),
			rectSize.y - innerMargin.y
		};
		app->render->DrawShape(selectedRect, true, SDL_Color(255, 255, 255, animationAlpha));
	}

	if (iconSize > 0 && character.portraitTextureID != -1)
	{
		iPoint cam = { -1 * app->render->GetCamera().x, -1 * app->render->GetCamera().y };

		drawPosition += cam;
		drawPosition.y += 2;
		app->render->DrawTexture(DrawParameters(character.portraitTextureID, drawPosition));
		drawPosition.y -= 2;
		drawPosition -= cam;
		drawPosition.x += iconSize;
	}

	drawPosition.x += innerMargin.x;

	app->fonts->DrawText(character.name, drawPosition);

	drawPosition.y += app->fonts->GetLineHeight(0) + innerMargin.y;

	app->fonts->DrawText(character.GetStatDisplay(BaseStats::LEVEL), drawPosition);

	drawPosition.x = originalPos.x + rectSize.x - innerMargin.x;
	drawPosition.x -= 32 - (drawPosition.x % 32);

	auto camera = app->render->GetCamera();

	int hpBarWidth = 220;
	int hpBarHeight = 32;
	int hpBarYOffset = 42;

	drawPosition.x -= (camera.x + hpBarWidth);
	drawPosition.y = originalPos.y + innerMargin.y - camera.y + hpBarYOffset;

	DrawHPBar(character.currentHP, character.stats[static_cast<int>(BaseStats::MAX_HP)], drawPosition, hpBarWidth, hpBarHeight);

	drawPosition.x += camera.x;
	drawPosition.y = originalPos.y + innerMargin.y + app->fonts->GetLineHeight(0)/2;

	app->fonts->DrawText("HP", TextParameters(1, DrawParameters(0, drawPosition)));
	//app->fonts->DrawText(character.GetStatDisplay(BaseStats::MAX_HP, true), drawPosition);
}

int GuiMenuList::MenuCharacter::GetHPBarTexture() const
{
	return hpBarTexture;
}


// I'm fucking canibalizing the code.
// I'm sad.

void GuiMenuList::MenuCharacter::DrawHPBar(int currentHP, int maxHP, iPoint pos, int hpBarWidth, int barHeight) const
{
	SDL_Rect hpBar{};	

	hpBar.x = pos.x;
	hpBar.y = pos.y;
	hpBar.w = hpBarWidth;
	hpBar.h = barHeight;

	hpBar.x-= 2;
	hpBar.y-= 2;
	hpBar.w+= 4;
	hpBar.h+= 4;

	app->render->DrawShape(hpBar, true, SDL_Color(0, 0, 0, 255));

	hpBar.x+= 2;
	hpBar.y+= 2;
	hpBar.w-= 4;
	hpBar.h-= 4;

	app->render->DrawShape(hpBar, true, SDL_Color(5, 8, 38, 255));

	float hp = static_cast<float>(currentHP) / static_cast<float>(maxHP);
	hpBar.w = hp > 0 ? static_cast<int>(hp * static_cast<float>(hpBar.w)) : 0;

	auto red = static_cast<uint8_t>(250.0f - (250.0f * hp));
	auto green = static_cast<uint8_t>(250.0f * hp);

	if(hpBarTexture == -1)
	{
		app->render->DrawShape(hpBar, true, SDL_Color(red, green, 0, 255));
	}
	else if(hp > 0)
	{
		if(hpBar.w > 1)
		{
			hpBar.w /= 2;
		}

		app->render->DrawShape(hpBar, true, SDL_Color(red, green, 0, 255));
		
		if (hpBar.w <= 0)
		{
			return;
		}

		hpBar.x += hpBar.w;

		auto camera = app->render->GetCamera();

		hpBar.x = camera.x + hpBar.x;
		hpBar.y =camera.y + hpBar.y;
			
		app->render->DrawGradientBar(
			hpBar,
			SDL_Color(red, green, 0, 255),
			SDL_Color(red, green, 122, 255),
			hpBar.w > 255 ? 255 : static_cast<uint8_t>(hpBar.w)
		);

	}
}

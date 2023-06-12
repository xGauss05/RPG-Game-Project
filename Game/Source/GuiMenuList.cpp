#include "GuiMenuList.h"
#include "App.h"

#include "Render.h"
#include "Input.h"
#include "TextManager.h"

#include "Log.h"

GuiMenuList::GuiMenuList() = default;

GuiMenuList::GuiMenuList(pugi::xml_node const& node) :
	position(iPoint(node.attribute("x").as_int(), node.attribute("y").as_int())),
	size(iPoint(node.attribute("width").as_int(), node.attribute("height").as_int()))
{
	if (auto itemNode = node.child("itemlist");
		itemNode)
	{
		
		fontID = itemNode.attribute("font") ? itemNode.attribute("font").as_int() : 0;
		maxColumns = itemNode.attribute("maxColumns") ? itemNode.attribute("maxColumns").as_int() : 1;

		auto GetMarginValues = [itemNode](std::string const& str)
		{
			if (auto strNode = itemNode.attribute(str.c_str());
				strNode)
			{
				std::string_view value = strNode.as_string();
				int xVal;
				std::string_view firstVal = value.substr(0, value.find_first_of(','));
				auto result = std::from_chars(firstVal.data(), firstVal.data() + firstVal.size(), xVal);
				int yVal;
				value.remove_prefix(value.find_first_of(',') + 1);
				auto result2 = std::from_chars(value.data(), value.data() + value.size(), yVal);

				if (result.ec == std::errc() && result2.ec == std::errc())
				{
					return iPoint(xVal, yVal);
				}

			}

			return iPoint(2, 2);
		};

		innerMargin = GetMarginValues("innerMargin");
		outterMargin = GetMarginValues("outterMargin") + background.segmentSize;

		iconSize = itemNode.attribute("iconSize").as_int();

		if (node.attribute("isonlyicons").as_bool())
		{
			m_itemSize = { iconSize , iconSize };
			size = m_itemSize;
			if (auto maxElementsAttr = node.attribute("maxelements");
				maxElementsAttr)
			{
				maxElements = maxElementsAttr.as_int();
				if (node.attribute("ishorizontal").as_bool())
				{
					size.x *= maxElements;
				}
				else
				{
					size.y *= maxElements;
				}
			}

			else
			{
				maxElements = size.y / m_itemSize.y;
			}
		}
		else
		{
			m_itemSize.x = size.x;
			m_itemSize.y = app->fonts->GetLineHeight(fontID) + (innerMargin.y * 2);

			if (iconSize + (innerMargin.y * 2) > m_itemSize.y)
			{
				m_itemSize.y = iconSize + (innerMargin.y * 2);
			}

			if (auto maxElementsAttr = itemNode.attribute("maxElements");
				maxElementsAttr)
			{
				maxElements = maxElementsAttr.as_int();
			}
			else
			{
				maxElements = size.y / m_itemSize.y;
			}
		}
	}
	if (node.attribute("hasbackground").as_bool())
	{
		SDL_Rect rect(
			position.x,
			position.y,
			size.x,
			size.y
		);

		background.SetPanelArea(rect);
	}

}

GuiMenuList::~GuiMenuList() = default;

void GuiMenuList::Initialize()
{
	InitializeElements();
	SetDefaultBooleanValues();
}

void GuiMenuList::Start()
{
	if (!items.empty())
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

void GuiMenuList::CreateMenuItem(std::string_view left, std::string_view center, std::string_view right, int textureID)
{
	items.emplace_back(std::make_unique<MenuItemBase>(left.data(), center.data(), right.data(), textureID));
}

void GuiMenuList::CreateMenuImage(int textureId, SDL_Rect const& srcRect)
{
	items.emplace_back(std::make_unique<MenuImage>(textureId, srcRect));
}

void GuiMenuList::DeleteMenuItem(int index)
{
	items.erase(items.begin() + index);
}

bool GuiMenuList::Draw() const
{
	background.Draw();


	if(!items.empty())
	{
		if (currentScroll > 0)
		{
			background.DrawTopArrow();
		}
		if (currentScroll + maxElements < items.size())
		{
			background.DrawArrow();
		}

		for (int i = currentScroll; i < currentScroll + maxElements && i < items.size(); ++i)
		{
			iPoint drawPosition(
				position.x + outterMargin.x,
				position.y + outterMargin.y
			);
			if (m_itemSize.x != size.x)
			{
				drawPosition.x += (m_itemSize.x * (i - currentScroll));
			}
			else
			{
				drawPosition.y += (m_itemSize.y * (i - currentScroll));
			}

			items[i]->Draw(drawPosition, iPoint(m_itemSize.x, m_itemSize.y), innerMargin, outterMargin, currentAlpha, iconSize, (currentItemSelected == i));
		}
	}

	return true;
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
	items.emplace_back(std::make_unique<MenuCharacter>(battler));
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
	else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
	{
		HandleRightButtonClick();
	}
	else if (app->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) // Controller equivalent :/
	{
		HandleControllerConfirm();
	}
	else if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		SelectAndScrollUpIfNeeded(1);
	}
	else if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		SelectAndScrollDownIfNeeded(1);
	}

	else if (app->input->controllerCount > 0)
	{
		if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_DOWN || app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_DOWN)
		{
			SelectAndScrollUpIfNeeded(1);
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_DOWN || app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_DOWN)
		{
			SelectAndScrollDownIfNeeded(1);
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
		{
			HandleControllerConfirm();
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_B) == KEY_DOWN)
		{
			HandleRightButtonClick();
		}
	}

	lastTimeSinceScrolled++;

	if (lastTimeSinceScrolled >= 10)
		HandleWheelScroll();
}

void GuiMenuList::HandleControllerConfirm()
{
	if (items.empty() || (currentItemSelected < 0 && currentItemSelected >= items.size()))
	{
		return;
	}

	SetLastClick(currentItemSelected);
	currentAlpha = 195;
	alphaDirection = -1;
	HandleLeftButtonClick(currentItemSelected);
}

void GuiMenuList::HandleLeftClick()
{
	if (items.empty())
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

		int elementClicked = m_itemSize.x != size.x ? 
			currentScroll + relativeCoords.x / m_itemSize.x : 
			currentScroll + relativeCoords.y / m_itemSize.y;

		int numOfItems = items.size();

		int elementInRange = MIN(
			elementClicked,
			currentScroll + maxElements - 1,		// Max index of item that are shown in screen
			{ numOfItems }					// Max number of available items
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
	int numOfItems = items.size();
	if (currentItemSelected < numOfItems - amount)
	{
		lastTimeSinceScrolled = 0;
		currentItemSelected += amount;
	}
	else
	{
		currentItemSelected = numOfItems - 1;
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
	int numOfItems = items.size();
	// If it can still scroll down
	if (currentScroll + maxElements < numOfItems)
	{
		lastTimeSinceScrolled = 0;


		if (currentScroll + amount >= maxElements)
			currentScroll = numOfItems - maxElements;
		else
			currentScroll += amount;
	}
}

void GuiMenuList::SetLastClick(int i)
{
	lastClick = i;
}

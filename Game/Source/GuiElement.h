#pragma once

#include "App.h"
#include "Input.h"

#include "Point.h"
#include "Defs.h"
#include "Log.h"

#include "Easing.h"

#include <functional>	//std::function

class GuiElement
{
public:

	GuiElement() = default;
	virtual ~GuiElement() = default;

	virtual int Update()
	{
		return 0;
	}

	virtual bool Draw() const
	{
		return true;
	}

	GuiElement *OnMouseEnter()
	{
		bIsHovered = true;

		MouseEnterHandler();

		return this;
	}

	virtual void MouseEnterHandler()
	{
		return;
	};

	void OnMouseLeave()
	{
		bIsHovered = false;

		MouseLeaveHandler();
	}

	virtual void MouseLeaveHandler()
	{
		return;
	};

	bool IsMouseHovering() const
	{
		uPoint mousePos = app->input->GetUnsignedMousePosition();

		if (mousePos.x >= currentPosition.x && mousePos.x <= currentPosition.x + size.x &&
			mousePos.y >= currentPosition.y && mousePos.y <= currentPosition.y + size.y)
		{
			return true;
		}

		return false;
	}

	void ToogleIsHovered()
	{
		bIsHovered = !bIsHovered;
	}

	void SetPosition(uint x, uint y)
	{
		currentPosition.x = x;
		currentPosition.y = y;
	}

	void SetPosition(uPoint pos)
	{
		currentPosition = pos;
	}

	uPoint GetCurrentPosition() const
	{
		return currentPosition;
	}
	uPoint GetStartingPosition() const
	{
		return startingPosition;
	}
	uPoint GetTargetPosition() const
	{
		return targetPosition;
	}

	bool HasEasing() const
	{
		return hasEasing;
	}

	void SetHasEasing(bool doesItHaveIt)
	{
		hasEasing = doesItHaveIt;
	}

	void ToggleEnable()
	{
		enabled = !enabled;
	}

	void Disable()
	{
		enabled = false;
	}

	void Enable()
	{
		enabled = true;
	}

	Easing GuiEasing;

protected:

	void Initialize(uPoint initialPos, uPoint targetPos, uPoint widthHeight)
	{
		currentPosition = initialPos;
		startingPosition = initialPos;
		targetPosition = targetPos;
		if (!(widthHeight % 32).IsZero())
		{
			widthHeight += uPoint(32, 32) - (widthHeight % 32);
		}
		size = widthHeight;
	}

	void Initialize(std::function<int()> const& funcPtr, uPoint initialPos, uPoint targetPos, uPoint widthHeight)
	{
		func = funcPtr;
		currentPosition = initialPos;
		startingPosition = initialPos;
		targetPosition = targetPos;
		if (!(widthHeight % 32).IsZero())
		{
			widthHeight += uPoint(32, 32) - (widthHeight % 32);
		}
		size = widthHeight;
	}

	int ExecuteFunction() const
	{
		return func();
	}

	uPoint GetSize() const
	{
		return size;
	}

	bool IsControllerHovered() const
	{
		return bIsHovered;
	}

	bool IsEnabled() const
	{
		return enabled;
	}

private:

	bool bIsHovered = false;		//If mouse is hovering

	bool enabled = true;

	std::function<int()> func;
	uPoint currentPosition = { 0, 0 };
	uPoint startingPosition = { 0, 0 };
	uPoint targetPosition = { 0,0 };
	uPoint size = { 0, 0 };
	bool hasEasing = false;

};

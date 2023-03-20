#pragma once

#include "App.h"
#include "Input.h"

#include "Point.h"
#include "Defs.h"
#include "Log.h"

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

		if (mousePos.x >= position.x && mousePos.x <= position.x + size.x &&
			mousePos.y >= position.y && mousePos.y <= position.y + size.y)
		{
			return true;
		}

		return false;
	}

protected:

	void Initialize(std::function<int()> const& funcPtr, uPoint pos, uPoint widthHeight)
	{
		func = funcPtr;
		position = pos;
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

	uPoint GetPosition() const
	{
		return position;
	}

	uPoint GetSize() const
	{
		return size;
	}

private:

	bool bIsHovered = false;		//If mouse is hovering

	std::function<int()> func;
	uPoint position = { 0, 0 };
	uPoint size = { 0, 0 };

};

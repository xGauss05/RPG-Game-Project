#pragma once

#include "App.h"
#include "Input.h"

#include "Point.h"
#include "Defs.h"

#include <functional>	//std::function

class GuiElement
{
public:

	GuiElement() = default;
	virtual ~GuiElement() = default;

	virtual void Update()
	{
		return;
	}

	virtual bool Draw()
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

	void Initialize(std::function<void()> const& funcPtr, uPoint pos, uPoint widthHeight)
	{
		func = funcPtr;
		position = pos;
		size = widthHeight;
	}

	void ExecuteFunction() const
	{
		func();
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

	std::function<void()> func;
	uPoint position = { 0, 0 };
	uPoint size = { 0, 0 };

};

#include "Window_Base.h"
#include "GuiButton.h"

#include "Log.h"

#include <utility>

Window_Base::Window_Base(pugi::xml_node const& node) :
	position({ node.attribute("x").as_uint(), node.attribute("y").as_uint() }),
	size({ node.attribute("width").as_uint(), node.attribute("height").as_uint() })
{
	InitializeFunctionPointerMap();

	for (auto const& child : node)
	{
		uPoint pos = { child.attribute("x").as_uint(), node.attribute("y").as_uint() };
		uPoint s = { child.attribute("width").as_uint(), node.attribute("height").as_uint() };
		std::string const text = child.attribute("text").as_string();

		if (auto result = strToFuncPtr.find(text);
			result != strToFuncPtr.end())
		{
			widgets.emplace_back(std::make_unique<GuiButton>(pos, s, text, result->second));
		}
		else
		{
			LOG("Widget %s function not found.", text);
			widgets.emplace_back(std::make_unique<GuiButton>(pos, s, text, strToFuncPtr.find("fallback")->second));
		}
	}
}

bool Window_Base::IsMouseHovering() const
{
	uPoint mousePos = app->input->GetUnsignedMousePosition();

	if (mousePos.x >= position.x && mousePos.x <= position.x + size.x &&
		mousePos.y >= position.y && mousePos.y <= position.y + size.y)
	{
		return true;
	}

	return false;
}

void Window_Base::InitializeFunctionPointerMap()
{	
	strToFuncPtr["fallback"] = std::bind_front(&Window_Base::FallbackFunction, this);
}

void Window_Base::FallbackFunction() const
{
	LOG("No function found for widget.");
}


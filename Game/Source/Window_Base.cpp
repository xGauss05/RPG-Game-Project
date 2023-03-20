#include "Window_Base.h"
#include "GuiButton.h"

#include "Log.h"

#include <utility>

Window_Base::Window_Base(pugi::xml_node const& node) :
	position({ node.attribute("x").as_uint(), node.attribute("y").as_uint() }),
	size({ node.attribute("width").as_uint(), node.attribute("height").as_uint() })
{
	AddFunctionToMap("Fallback", std::bind_front(&Window_Base::FallbackFunction, this));
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

void Window_Base::Draw() const
{
	for (auto const &elem : widgets)
	{
		elem->Draw();
	}
}

int Window_Base::Update() const
{
	for (auto const& elem : widgets)
	{
		if (auto result = elem->Update();
			result != 0)
		{
			return result;
		}
	}
	return 0;
}

void Window_Base::CreateButtons(pugi::xml_node const& node)
{
	for (auto const& child : node.children("button"))
	{
		uPoint pos = { child.attribute("x").as_uint(), child.attribute("y").as_uint() };
		uPoint s = { child.attribute("width").as_uint(), child.attribute("height").as_uint() };
		const std::string name = child.attribute("text").as_string();
		const std::string fun = child.attribute("function").as_string();

		auto result = strToFuncPtr.find(fun);

		if (result == strToFuncPtr.end())
		{
			LOG("Widget [Play] function not found.", name.c_str());
			result = strToFuncPtr.find("Fallback");
		}

		std::vector<SDL_Rect> buttonStateTexture;

		for (auto const& state : child.children("state_texture"))
		{
			buttonStateTexture.emplace_back(
				state.attribute("x").as_int(),
				state.attribute("y").as_int(),
				child.attribute("segmentwidth").as_int(),
				child.attribute("segmentheight").as_int()
			);
		}

		widgets.emplace_back(std::make_unique<GuiButton>(pos, s, name, result->second, buttonStateTexture));
	}
}

void Window_Base::AddFunctionToMap(std::string const& str, std::function<int()> const &funcPtr)
{
	strToFuncPtr[str] = funcPtr;
}

int Window_Base::FallbackFunction() const
{
	LOG("No function found for widget.");
	return 0;
}


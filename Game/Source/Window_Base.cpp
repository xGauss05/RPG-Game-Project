#include "Window_Base.h"
#include "GuiButton.h"
#include "GuiBox.h"
#include "GuiCheckbox.h"

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

int Window_Base::Update()
{
	return UpdateWidgets();
}

void Window_Base::CreateButtons(pugi::xml_node const& node)
{
	for (auto const& child : node.children("button"))
	{
		uPoint startingPos = { child.attribute("startingX").as_uint(), child.attribute("startingY").as_uint() };
		uPoint targetPos = { child.attribute("targetX").as_uint(), child.attribute("targetY").as_uint() };
		uPoint s = { child.attribute("width").as_uint(), child.attribute("height").as_uint() };
		const std::string name = child.attribute("text").as_string();
		const std::string fun = child.attribute("function").as_string();

		auto result = strToFuncPtr.find(fun);

		if (result == strToFuncPtr.end())
		{
			LOG("Widget function not found.", name.c_str());
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

		widgets.emplace_back(std::make_unique<GuiButton>(startingPos, targetPos, s, name, result->second, buttonStateTexture));
	}
}

void Window_Base::CreatePanels(pugi::xml_node const& node)
{
	for (auto const& panel : node.children("panel"))
	{
		uPoint startingPos = { panel.attribute("startingX").as_uint(), panel.attribute("startingY").as_uint() };
		uPoint targetPos = { panel.attribute("targetX").as_uint(), panel.attribute("targetY").as_uint() };
		uPoint area = { panel.attribute("width").as_uint(), panel.attribute("height").as_uint() };
		
		auto texProperties = panel.child("texture_properties");
		//This is position of the first tile in the UI sheet and size of each tile.
		SDL_Rect rect =
		{ 
			texProperties.attribute("tileX").as_int(),
			texProperties.attribute("tileY").as_int(),
			texProperties.attribute("tileW").as_int(),
			texProperties.attribute("tileH").as_int()
		};

		int advance = texProperties.attribute("advance").as_int();
		iPoint segments =
		{ 
			texProperties.attribute("horizontalsegments").as_int(),
			texProperties.attribute("verticalsegments").as_int()
		};
		
		//This path could be changed to a dialogue-exclusive XML file
		auto text = panel.child("text").attribute("chatBoxText").as_string();

		widgets.emplace_back(std::make_unique<GuiBox>(startingPos, targetPos, area, rect, advance, 0, segments, text));
	}
}

void Window_Base::CreateCheckboxes(pugi::xml_node const& node)
{
	for (auto const& child : node.children("checkbox"))
	{
		uPoint startingPos = { child.attribute("startingX").as_uint(), child.attribute("startingY").as_uint() };
		uPoint targetPos = { child.attribute("targetX").as_uint(), child.attribute("targetY").as_uint() };
		uPoint s = { child.attribute("width").as_uint(), child.attribute("height").as_uint() };
		const std::string name = child.attribute("text").as_string();
		const std::string fun = child.attribute("function").as_string();

		auto result = strToFuncPtr.find(fun);

		if (result == strToFuncPtr.end())
		{
			LOG("Widget function not found.", name.c_str());
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

		widgets.emplace_back(std::make_unique<GuiCheckbox>(startingPos, targetPos, s, name, result->second, buttonStateTexture));
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

int Window_Base::UpdateWidgets()
{
	for (int i = 0; auto const& elem : widgets)
	{
		if (auto result = elem->Update();
			result != 0)
		{
			lastWidgetInteractedIndex = i;
			return result;
		}
		i++;
	}
	return 0;
}

size_t Window_Base::GetNumberWidgets() const
{
	return widgets.size();
}

void Window_Base::ControllerHoveringWidget(int index)
{
	if (index < widgets.size())	
		widgets[index]->ToogleIsHovered();
}

GuiElement* Window_Base::AccessLastWidget()
{
	if(widgets.empty())
		return nullptr;
	return widgets.back().get();
}


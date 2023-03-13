#pragma once

#include "GuiElement.h"
#include "Textures.h"

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

class Window_Base
{
public:

	explicit Window_Base(pugi::xml_node const &node);

	bool IsMouseHovering() const;

protected:

	void InitializeFunctionPointerMap();

	// If string is not found in pointer map, it fallbacks to this
	void FallbackFunction() const;

private:
	uPoint position = { 0, 0 };
	uPoint size = { 0, 0 };
	SDL_Color color = { 255,255,255,50};

	bool bActive = false;
	bool bVisible = false;

	std::shared_ptr<SDL_Texture> background = nullptr;
	std::vector<std::unique_ptr<GuiElement>> widgets;

	std::unordered_map<std::string, std::function<void()>, StringHash, std::equal_to<>> strToFuncPtr;
};


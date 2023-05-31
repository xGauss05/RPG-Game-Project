#ifndef __WINDOW_MENU_LIST_ITEM_H__
#define __WINDOW_MENU_LIST_ITEM_H__

#include "Render.h"
#include "TextManager.h"

#include <memory>
#include <string>

constexpr uint8_t g_guiItemMinAlpha = 5;
constexpr uint8_t g_guiItemMaxAlpha = 200;

struct Battler;

class MenuItem
{
public:
	virtual ~MenuItem() = default;

	virtual void Draw(
		iPoint originalPos,
		iPoint rectSize,
		iPoint innerMargin = iPoint(0, 0),
		[[maybe_unused]] iPoint outMargin = iPoint(0, 0),
		Uint8 animationAlpha = 0,
		int iconSize = 0,
		bool currentlySelected = false
	) const = 0;
};

class MenuItemBase : public MenuItem
{
public:

	MenuItemBase() = default;
	explicit MenuItemBase(
		std::string const& leftText,
		std::string const& centerText,
		std::string const& rightText,
		int textureID = -1
	);
	~MenuItemBase() override = default;

	void Draw(
		iPoint originalPos,
		iPoint rectSize,
		iPoint innerMargin = iPoint(0, 0),
		iPoint outMargin = iPoint(0, 0),
		Uint8 animationAlpha = 0,
		int iconSize = 0,
		bool currentlySelected = false
	) const override;

private:
	struct TextLocation
	{
		std::string leftText = "";
		std::string centerText = "";
		std::string rightText = "";
	};
	TextLocation text;
	int iconTexture = -1;
};

class MenuCharacter : public MenuItem
{
public:
	MenuCharacter() = default;
	explicit MenuCharacter(Battler const& battler);
	~MenuCharacter() override = default;

	void Draw(
		iPoint originalPos,
		iPoint rectSize,
		iPoint innerMargin = iPoint(0, 0),
		iPoint outMargin = iPoint(0, 0),
		Uint8 animationAlpha = 0,
		int iconSize = 0,
		bool currentlySelected = false
	) const override;

private:
	void DrawHPBar(int currentHP, int maxHP, iPoint pos, int hpBarWidth, int barHeight = 32) const;
	void DrawManaBar(int currentMana, int maxMana, iPoint pos, int manaBarWidth, int manaBarHeight = 32) const;

	const Battler& character;
};


class MenuImage : public MenuItem
{
public:
	MenuImage() = default;
	explicit MenuImage(int texId, SDL_Rect const& srcRect);
	~MenuImage() override = default;

	void Draw(
		iPoint originalPos,
		iPoint rectSize,
		iPoint innerMargin = iPoint(0, 0),
		iPoint outMargin = iPoint(0, 0),
		Uint8 animationAlpha = 0,
		int iconSize = 0,
		bool currentlySelected = false
	) const override;

private:
	int textureId = -1;
	SDL_Rect rect = { 0, 0, 0, 0 };
};

#endif //__WINDOW_MENU_LIST_ITEM_H__
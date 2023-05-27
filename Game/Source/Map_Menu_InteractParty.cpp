#include "Map_Menu_InteractParty.h"

#include "Render.h"

#include "Log.h"

Map_Menu_InteractParty::Map_Menu_InteractParty(pugi::xml_node const& node)
	: GuiMenuList(node)
{}

void Map_Menu_InteractParty::HandleLeftButtonClick(int result)
{
	SetClickHandled(true);
}

void Map_Menu_InteractParty::HandleRightButtonClick()
{
	ResetCurrentItemSelected();
	SetDeleteMenu(true);
	SetClickHandled(true);
}

void Map_Menu_InteractParty::InitializeElements()
{
	ClearMenuItems();

	for (auto const& elem : playerParty->party)
	{
		CreateMenuItem(MenuCharacter(elem));
	}
}

Map_Menu_InteractParty::MenuCharacter::MenuCharacter(Battler const& battler)
	: character(battler)
{
}

void Map_Menu_InteractParty::MenuCharacter::Draw(iPoint originalPos, iPoint rectSize, iPoint innerMargin, iPoint outMargin, uint8_t animationAlpha, int iconSize, bool currentlySelected) const
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
	}

	drawPosition.x += character.portraitTextureID + innerMargin.x;
}

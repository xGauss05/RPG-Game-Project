#include "GuiMenuList.h"
#include "App.h"
#include "Render.h"
#include "TextManager.h"
#include "Log.h"

GuiMenuList::~GuiMenuList()
{
	panel->Unload();
}

GuiMenuList::GuiMenuList(uPoint pos, uPoint widthHeight, SDL_Rect const& rect, int advance, iPoint tSegments)
{
	Initialize(pos, pos, widthHeight);

	int textureID = app->tex->Load("Assets/UI/GUI_4x_sliced.png");
	panel = std::make_unique<GuiPanelSegmented>(rect, advance, textureID, tSegments);
}

bool GuiMenuList::Draw() const
{
	auto centerPoint = iPoint(GetCurrentPosition().x, GetCurrentPosition().y);

	panel->Draw(centerPoint, iPoint(GetSize().x, GetSize().y));

	return true;
}

void GuiMenuList::DebugDraw() const
{
	SDL_Rect debugRect(GetCurrentPosition().x, GetCurrentPosition().y, GetSize().x, GetSize().y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

#include "GuiBox.h"
#include "App.h"
#include "Render.h"
#include "Input.h"
#include "TextManager.h"
#include "Audio.h"
#include "Log.h"

#include <map>

GuiBox::~GuiBox()
{
	panel->Unload();
}

GuiBox::GuiBox(uPoint pos, uPoint widthHeight,SDL_Rect const& rect,int advance,int id,iPoint tSegments)
{
	Initialize(pos, widthHeight);

	int textureID = app->tex->Load("Assets/UI/GUI_4x_sliced.png");
	panel = std::make_unique<GuiPanelSegmented>(rect, advance, textureID, tSegments);
}

bool GuiBox::Draw() const
{
	

	return true;
}

void GuiBox::DebugDraw() const
{
	SDL_Rect debugRect(GetPosition().x, GetPosition().y, GetSize().x, GetSize().y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

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

GuiBox::GuiBox(uPoint startingPos, uPoint targetPos, uPoint widthHeight,SDL_Rect const& rect,int advance,int id, iPoint tSegments, std::string const &t)
	: text(t)
{
	Initialize(startingPos, targetPos, widthHeight);

	int textureID = app->tex->Load("Assets/UI/GUI_4x_sliced.png");
	panel = std::make_unique<GuiPanelSegmented>(rect, advance, textureID, tSegments);
}

bool GuiBox::Draw() const
{
	auto centerPoint = iPoint(GetCurrentPosition().x, GetCurrentPosition().y);

	panel->Draw(centerPoint, iPoint(GetSize().x, GetSize().y));

	if(!text.empty())
	{
		centerPoint += iPoint(GetSize().x / 2, GetSize().y / 2);

		//THIS IS WHAT WAS NOT WORKING
		//TextParameters params(0, DrawParameters(0, centerPoint));
		//params.align = AlignTo::ALIGN_CENTER;

		app->fonts->DrawText(text, TextParameters(0, DrawParameters(0, centerPoint)).Align(AlignTo::ALIGN_CENTER));
	}

	return true;
}

std::string_view GuiBox::GetText()
{
	return text;
}

void GuiBox::ModifyText(std::string_view newText)
{
	text = newText;
}

void GuiBox::DebugDraw() const
{
	SDL_Rect debugRect(GetCurrentPosition().x, GetCurrentPosition().y, GetSize().x, GetSize().y);

	app->render->DrawShape(debugRect, false, SDL_Color(255, 0, 0, 255));
}

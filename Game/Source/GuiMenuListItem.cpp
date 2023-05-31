#include "GuiMenuListItem.h"
#include "GameParty.h"

MenuItemBase::MenuItemBase(std::string const &leftText, std::string const& centerText, std::string const& rightText, int textureID)
	: text({ leftText, centerText, rightText }), iconTexture(textureID)
{}

void MenuItemBase::Draw(iPoint originalPos, iPoint rectSize, iPoint inMargin, iPoint outMargin, Uint8 animationAlpha, int sizeIcon, bool currentlySelected) const
{
	iPoint drawPosition = originalPos + inMargin;

	if (currentlySelected)
	{
		iPoint cam = { app->render->GetCamera().x, app->render->GetCamera().y };
		SDL_Rect selectedRect = {
			(-1 * cam.x) + originalPos.x,
			(-1 * cam.y) + originalPos.y,
			rectSize.x, 
			rectSize.y
		};
		app->render->DrawShape(selectedRect, true, SDL_Color(255, 255, 255, animationAlpha));
	}

	if (sizeIcon > 0)
	{
		if (iconTexture != -1)
		{
			iPoint cam ={ -1 * app->render->GetCamera().x, -1 * app->render->GetCamera().y };
			
			drawPosition += cam;
			app->render->DrawTexture(DrawParameters(iconTexture, drawPosition));
			drawPosition -= cam;
		}

		drawPosition.x += sizeIcon + inMargin.x;
	}
	drawPosition.y = originalPos.y + (rectSize.y / 2);

	if (!text.leftText.empty())
	{
		app->fonts->DrawText(
			text.leftText,
			TextParameters(
				0,
				DrawParameters(0, drawPosition)
			).Align(AlignTo::ALIGN_CENTER_LEFT)
		);
	}

	if (!text.centerText.empty())
	{
		// Center the draw position
		drawPosition.x = originalPos.x + (rectSize.x / 2);

		app->fonts->DrawText(
			text.centerText,
			TextParameters(
				0,
				DrawParameters(0, drawPosition)
			).Align(AlignTo::ALIGN_CENTER)
		);
	}

	if (!text.rightText.empty())
	{
		drawPosition.x = originalPos.x + rectSize.x - inMargin.x;
		
		app->fonts->DrawText(
			text.rightText,
			TextParameters(
				0,
				DrawParameters(0, drawPosition)
			).Align(AlignTo::ALIGN_CENTER_RIGHT)
		);
	}
}

MenuCharacter::MenuCharacter(Battler const& battler)
	: character(battler)
{}

void MenuCharacter::Draw(iPoint originalPos, iPoint rectSize, iPoint inMargin, iPoint outMargin, Uint8 animationAlpha, int iconSiz, bool currentlySelected) const
{
	iPoint drawPosition = originalPos + inMargin;

	if (currentlySelected)
	{
		iPoint cam = { app->render->GetCamera().x, app->render->GetCamera().y };
		SDL_Rect selectedRect = {
			(-1 * cam.x) + originalPos.x,
			(-1 * cam.y) + originalPos.y,
			rectSize.x,
			rectSize.y
		};
		app->render->DrawShape(selectedRect, true, SDL_Color(255, 255, 255, animationAlpha));
	}

	if (iconSiz > 0 && character.portraitTextureID != -1)
	{
		iPoint cam = { -1 * app->render->GetCamera().x, -1 * app->render->GetCamera().y };
		drawPosition += cam;

		app->render->DrawTexture(DrawParameters(character.portraitTextureID, drawPosition));
		drawPosition -= cam;
		drawPosition.x += iconSiz + inMargin.x;
	}


	app->fonts->DrawText(character.name, drawPosition);

	drawPosition.y += app->fonts->GetLineHeight(0) + inMargin.y;

	app->fonts->DrawText(character.GetStatDisplay(BaseStats::LEVEL), drawPosition);

	drawPosition.x = originalPos.x + rectSize.x - inMargin.x - outMargin.x;

	auto camera = app->render->GetCamera();

	int hpBarWidth = 220;
	int hpBarHeight = 32;
	int hpBarYOffset = 42;

	drawPosition.x -= (camera.x + hpBarWidth);
	drawPosition.y = originalPos.y + inMargin.y - camera.y + hpBarYOffset;

	DrawHPBar(character.currentHP, character.GetStat(BaseStats::MAX_HP), drawPosition, hpBarWidth, hpBarHeight);

	drawPosition.y += hpBarYOffset;

	DrawManaBar(character.currentMana, character.GetStat(BaseStats::MAX_MANA), drawPosition, hpBarWidth, hpBarHeight);

	drawPosition.x += camera.x;
	drawPosition.y = originalPos.y + inMargin.y + hpBarYOffset - app->fonts->GetLineHeight(0)/2;

	app->fonts->DrawText("HP", TextParameters(1, DrawParameters(0, drawPosition)));

	drawPosition.y += hpBarYOffset;

	app->fonts->DrawText("MP", TextParameters(1, DrawParameters(0, drawPosition)));

	drawPosition.x += hpBarWidth - 6;
	drawPosition.y -= (hpBarYOffset + 2);

	app->fonts->DrawText(character.GetStatDisplay(BaseStats::MAX_HP, true), TextParameters(0, DrawParameters(0, drawPosition)).Align(AlignTo::ALIGN_TOP_RIGHT));

	drawPosition.y += hpBarYOffset;

	app->fonts->DrawText(character.GetStatDisplay(BaseStats::MAX_MANA, true), TextParameters(0, DrawParameters(0, drawPosition)).Align(AlignTo::ALIGN_TOP_RIGHT));
}

void MenuCharacter::DrawHPBar(int currentHP, int maxHP, iPoint pos, int hpBarWidth, int barHeight) const
{
	SDL_Rect hpBar{};	

	hpBar.x = pos.x;
	hpBar.y = pos.y;
	hpBar.w = hpBarWidth;
	hpBar.h = barHeight;

	hpBar.x-= 2;
	hpBar.y-= 2;
	hpBar.w+= 4;
	hpBar.h+= 4;

	app->render->DrawShape(hpBar, true, SDL_Color(0, 0, 0, 255));

	hpBar.x+= 2;
	hpBar.y+= 2;
	hpBar.w-= 4;
	hpBar.h-= 4;

	app->render->DrawShape(hpBar, true, SDL_Color(5, 8, 38, 255));

	float hp = static_cast<float>(currentHP) / static_cast<float>(maxHP);
	hpBar.w = hp > 0 ? static_cast<int>(hp * static_cast<float>(hpBar.w)) : 0;

	auto red = static_cast<uint8_t>(250.0f - (250.0f * hp));
	auto green = static_cast<uint8_t>(250.0f * hp);

	if(hp > 0)
	{
		if(hpBar.w > 1)
		{
			hpBar.w /= 2;
		}

		app->render->DrawShape(hpBar, true, SDL_Color(red, green, 0, 255));
		
		if (hpBar.w <= 0)
		{
			return;
		}

		hpBar.x += hpBar.w;

		auto camera = app->render->GetCamera();

		hpBar.x = camera.x + hpBar.x;
		hpBar.y =camera.y + hpBar.y;
			
		app->render->DrawGradientBar(
			hpBar,
			SDL_Color(red, green, 0, 255),
			SDL_Color(red, green, 122, 255),
			hpBar.w > 255 ? 255 : static_cast<uint8_t>(hpBar.w)
		);

	}
}

void MenuCharacter::DrawManaBar(int currentMana, int maxMana, iPoint pos, int manaBarWidth, int manaBarHeight) const
{
	SDL_Rect manaBar{};	

	manaBar.x = pos.x;
	manaBar.y = pos.y;
	manaBar.w = manaBarWidth;
	manaBar.h = manaBarHeight;

	manaBar.x-= 2;
	manaBar.y-= 2;
	manaBar.w+= 4;
	manaBar.h+= 4;

	app->render->DrawShape(manaBar, true, SDL_Color(0, 0, 0, 255));

	manaBar.x+= 2;
	manaBar.y+= 2;
	manaBar.w-= 4;
	manaBar.h-= 4;

	app->render->DrawShape(manaBar, true, SDL_Color(5, 8, 38, 255));

	float hp = static_cast<float>(currentMana) / static_cast<float>(maxMana);
	manaBar.w = hp > 0 ? static_cast<int>(hp * static_cast<float>(manaBar.w)) : 0;

	SDL_Color startColor = { 0, 0, 184, 255 };
	SDL_Color endColor = { 24, 75, 184, 255 };

	if(hp > 0)
	{
		if(manaBar.w > 1)
		{
			manaBar.w /= 2;
		}

		app->render->DrawShape(manaBar, true, startColor);
		
		if (manaBar.w <= 0)
		{
			return;
		}

		manaBar.x += manaBar.w;

		auto camera = app->render->GetCamera();

		manaBar.x = camera.x + manaBar.x;
		manaBar.y =camera.y + manaBar.y;
			
		app->render->DrawGradientBar(
			manaBar,
			startColor,
			endColor,
			manaBar.w > 255 ? 255 : static_cast<uint8_t>(manaBar.w)
		);

	}
}

MenuImage::MenuImage(int texId, SDL_Rect const &srcRect)
	: textureId(texId), rect(srcRect)
{}

void MenuImage::Draw(iPoint originalPos, iPoint rectSize, iPoint innerMargin, iPoint outMargin, Uint8 animationAlpha, int iconSize, bool currentlySelected) const
{
	iPoint camera = { app->render->GetCamera().x, app->render->GetCamera().y };
	iPoint drawPosition = originalPos - camera;

	app->render->DrawTexture(DrawParameters(textureId, drawPosition).Section(&rect));

	if (currentlySelected)
	{
		SDL_Rect selectedRect = {
			drawPosition.x,
			drawPosition.y,
			rectSize.x,
			rectSize.y
		};
		app->render->DrawShape(selectedRect, true, SDL_Color(255, 255, 255, animationAlpha));
	}
}

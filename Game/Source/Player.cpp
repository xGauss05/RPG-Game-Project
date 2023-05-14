#include "App.h"
#include "Player.h"

#include "Map.h"

#include "Input.h"
#include "Render.h"

Player::Player() = default;

Player::~Player() = default;

void Player::SetPosition(iPoint newPosition)
{
	position = newPosition;
	app->render->AdjustCamera(position);
}

void Player::SetSpeed(int speed)
{
	this->speed = speed;
}

iPoint Player::GetPosition() const
{
	return position;
}

bool Player::FinishedMoving() const
{
	return moveVector.IsZero();
}

void Player::Draw() const
{
	DrawParameters drawPlayer(GetTextureID(), position);
	drawPlayer.Scale(fPoint(3, 3));
	app->render->DrawTexture(drawPlayer.Section(&currentSpriteSlice));
}

void Player::Create()
{
	app->tex->Load("Assets/Textures/Main_Characters/Antonio/Antonio.png");	//This should not be done like this, all of this should load from the map XML.
	Sprite::Initialize("Assets/Textures/Main_Characters/Antonio/Antonio.png", 0);
	
	if(position.IsZero())
		position = { 192, 2304 }; // 64 * 3 (192), 768 * 3 (2358)

	size = { 48, 96 }; //16 * 3, 32 * 3
	currentSpriteSlice = {
		(GetTextureIndex().x + 1) * (size.x / 3),
		GetTextureIndex().y * (size.y / 3),
		size.x / 3,
		size.y / 3
	};
}

Player::PlayerAction Player::HandleInput() const
{
	using enum KeyState;
	using enum Player::PlayerAction::Action;
	auto positionCheck = position;
	positionCheck.y += tileSize;
	PlayerAction returnAction = { positionCheck, NONE };

	if (!moveVector.IsZero())
		return returnAction;

	if (app->input->controllerCount > 0)
	{
		if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_REPEAT)
		{
			returnAction.action |= MOVE;
			returnAction.destinationTile.y -= tileSize;
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_REPEAT)
		{
			returnAction.action |= MOVE;
			returnAction.destinationTile.x -= tileSize;
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_REPEAT)
		{
			returnAction.action |= MOVE;
			returnAction.destinationTile.y += tileSize;

		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_REPEAT)
		{
			returnAction.action |= MOVE;
			returnAction.destinationTile.x += tileSize;
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_X) == KEY_DOWN)
		{
			returnAction.action |= INTERACT;
		}
	}
	else
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			returnAction.action |= MOVE;
			returnAction.destinationTile.y -= tileSize;
		}
		else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			returnAction.action |= MOVE;
			returnAction.destinationTile.x -= tileSize;
		}
		else if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			returnAction.action |= MOVE;
			returnAction.destinationTile.y += tileSize;

		}
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			returnAction.action |= MOVE;
			returnAction.destinationTile.x += tileSize;
		}
		else if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		{
			returnAction.action |= INTERACT;
		}
	}

	return returnAction;
}

void Player::StartAction(PlayerAction playerAction)
{
	if (playerAction.action == PlayerAction::Action::MOVE)
	{
		StartMovement();
	}
}

void Player::StartMovement()
{
	using enum KeyState;

	if (app->input->controllerCount > 0)
	{
		if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_REPEAT)
		{
			moveVector.y = -1;
			currentSpriteSlice.y = (GetTextureIndex().y + 3) * (size.y / 3);
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_REPEAT)
		{
			moveVector.x = -1;
			currentSpriteSlice.y = (GetTextureIndex().y + 1) * (size.y / 3);
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_REPEAT)
		{
			moveVector.y = 1;
			currentSpriteSlice.y = GetTextureIndex().y * (size.y / 3);

		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_REPEAT)
		{
			moveVector.x = 1;
			currentSpriteSlice.y = (GetTextureIndex().y + 2) * (size.y / 3);
		}
	}
	else
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			moveVector.y = -1;
			currentSpriteSlice.y = (GetTextureIndex().y + 3) * (size.y / 3);
		}
		else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			moveVector.x = -1;
			currentSpriteSlice.y = (GetTextureIndex().y + 1) * (size.y / 3);
		}
		else if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			moveVector.y = 1;
			currentSpriteSlice.y = GetTextureIndex().y * (size.y / 3);
		}
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			moveVector.x = 1;
			currentSpriteSlice.y = (GetTextureIndex().y + 2) * (size.y / 3);
		}
	}

	lastDir = moveVector;
}

void Player::Update()
{
	if (!moveVector.IsZero())
	{
		AnimateMove();
		SmoothMove();
		app->render->AdjustCamera(position);
	}
}

void Player::AnimateMove()
{
	if (animTimer == 8)
	{
		currentSpriteSlice.x += (size.x / 3);
		if (currentSpriteSlice.x == (size.x / 3) * (GetTextureIndex().x + 3))
		{
			currentSpriteSlice.x = GetTextureIndex().x * (size.x / 3);
		}
		animTimer = 0;
	}
	else
	{
		animTimer++;
	}
}

void Player::SmoothMove()
{
	if (moveTimer == timeForATile)
	{
		moveTimer = 0;
		position += (moveVector * speed);
		if (position.x % tileSize == 0 && position.y % tileSize == 0)
		{
			moveVector.SetToZero();
		}
	}
	else
	{
		moveTimer++;
	}
}

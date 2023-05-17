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

	drawPosition = position;
	drawPosition.y -= tileSize;

	app->render->AdjustCamera(position);
}

iPoint Player::GetDrawPosition() const
{
	return drawPosition;
}

void Player::SetSpeed(int newSpeed)
{
	speed = newSpeed;
}

bool Player::IsStandingStill() const
{
	return moveVector.IsZero();
}

void Player::SetMovementStopped(bool b)
{
	movementStopped = b;
}

bool Player::GetMovementStopped() const
{
	return movementStopped;
}

void Player::Draw() const
{
	DrawParameters drawPlayer(GetTextureID(), drawPosition);
	app->render->DrawTexture(drawPlayer.Section(&currentSpriteSlice));
}

void Player::Create()
{
	app->tex->Load("Assets/Textures/Main_Characters/Antonio/Antonio.png");	//This should not be done like this, all of this should load from the map XML.
	Sprite::Initialize("Assets/Textures/Main_Characters/Antonio/Antonio.png", 0);
	
	if(position.IsZero())
		position = { 192, 2304 };

	size = { 48, 96 };

	drawPosition = position;
	drawPosition.y -= tileSize;

	currentSpriteSlice = {
		(GetTextureIndex().x + 1) * size.x,
		GetTextureIndex().y * size.y,
		size.x,
		size.y
	};
}

Player::PlayerAction Player::HandleInput() const
{
	using enum KeyState;
	using enum Player::PlayerAction::Action;
	PlayerAction returnAction = { position, NONE };

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

void Player::StartMovementIfAble(bool walkable)
{
	// We start the movement no matter what, as it holds the input handler
	StartMovement();

	// Although, if the tile it's non-walkable, we stop the velocity so we get the rotated sprite without moving
	if (!walkable)
		moveVector.SetToZero();
	
}

void Player::StartMovement()
{
	using enum KeyState;

	if (app->input->controllerCount > 0)
	{
		if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_REPEAT)
		{
			moveVector.y = -1;
			currentSpriteSlice.y = (GetTextureIndex().y + 3) * size.y;
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == KEY_REPEAT)
		{
			moveVector.x = -1;
			currentSpriteSlice.y = (GetTextureIndex().y + 1) * size.y;
		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_REPEAT)
		{
			moveVector.y = 1;
			currentSpriteSlice.y = GetTextureIndex().y * size.y;

		}
		else if (app->input->GetControllerKey(0, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == KEY_REPEAT)
		{
			moveVector.x = 1;
			currentSpriteSlice.y = (GetTextureIndex().y + 2) * size.y;
		}
	}
	else
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			moveVector.y = -1;
			currentSpriteSlice.y = (GetTextureIndex().y + 3) * size.y;
		}
		else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			moveVector.x = -1;
			currentSpriteSlice.y = (GetTextureIndex().y + 1) * size.y;
		}
		else if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			moveVector.y = 1;
			currentSpriteSlice.y = GetTextureIndex().y * size.y;
		}
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			moveVector.x = 1;
			currentSpriteSlice.y = (GetTextureIndex().y + 2) * size.y;
		}
	}

	lastDir = moveVector;
}

void Player::RotatePlayer()
{
	StartMovement();
	moveVector.SetToZero();
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
		currentSpriteSlice.x += size.x;
		if (currentSpriteSlice.x == size.x * (GetTextureIndex().x + 3))
		{
			currentSpriteSlice.x = GetTextureIndex().x * (size.x);
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
		drawPosition += (moveVector * speed);
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

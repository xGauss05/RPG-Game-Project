#include "App.h"
#include "Player.h"

#include "Map.h"

#include "Input.h"
#include "Render.h"

#include "Log.h"

#include <bitset>
#include <cstddef>

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

	movementControls.emplace(SDL_SCANCODE_W, FORWARD_REPEAT);
	movementControls.emplace(SDL_SCANCODE_S, BACKWARD_REPEAT);
	movementControls.emplace(SDL_SCANCODE_A, LEFT_REPEAT);
	movementControls.emplace(SDL_SCANCODE_D, RIGHT_REPEAT);
	actionControls.emplace(SDL_SCANCODE_E, INTERACT_REPEAT);
	actionControls.emplace(SDL_SCANCODE_SPACE, INTERACT_REPEAT);
}

Player::PlayerAction Player::HandleInput()
{
	using enum KeyState;
	using enum PlayerMoveControlsToBind;
	using enum Player::PlayerAction::Action;
	PlayerAction returnAction = { position, NONE };

	if (!moveVector.IsZero())
		return returnAction;


	auto IsNotZero = [](uint16_t num) { return ((num | (~num + 1)) >> 15) & 1; };

	for (uint16_t movementKeysFlags = GetActionsKeysPressed(), repeatHelper = FORWARD_REPEAT;
		(movementKeysFlags) && IsNotZero(repeatHelper);
		repeatHelper <<= 1)
	{
		uint16_t pressedKeys = repeatHelper & movementKeysFlags;

		if (!IsNotZero(pressedKeys))
		{
			continue;
		}

		if (pressedKeys & ANY_KEY_REPEAT)
		{
			returnAction.action |= MOVE;

			if(auto [it, success] = actionQueue.insert(CastToEnum(ANY_KEY_IDLE & repeatHelper));
				success)
			{
				LOG("Repeat: %i", static_cast<uint16_t>(ANY_KEY_REPEAT & repeatHelper));
			}
		}

		if (pressedKeys & ANY_KEY_UP)
		{
			if (auto success = actionQueue.erase(CastToEnum((ANY_KEY_IDLE & repeatHelper) >> 2));
				success)
			{
				LOG("Released: %i", static_cast<uint16_t>(ANY_KEY_UP & repeatHelper) >> 2);
			}
		}	

		if ((pressedKeys & ANY_KEY_IDLE))
		{
			if (auto success = actionQueue.erase(CastToEnum((ANY_KEY_IDLE & repeatHelper) >> 3));
				success)
			{
				LOG("Released: %i", static_cast<uint16_t>(ANY_KEY_IDLE & repeatHelper) >> 3);
			}
		}	

		movementKeysFlags ^= repeatHelper;
	}

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

void Player::StartOrRotateMovement(bool walkable)
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

Player::PlayerMoveControlsToBind Player::CastToEnum(uint16_t value) const
{
	return static_cast<PlayerMoveControlsToBind>(value);
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
		currentSpriteSlice.x += (size.x * animIncrease);

		if (currentSpriteSlice.x == size.x * (GetTextureIndex().x + 2) || (currentSpriteSlice.x <= size.x * GetTextureIndex().x))
		{
			animIncrease *= -1;
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

uint16_t Player::GetActionsKeysPressed() const
{
	uint16_t actionsPressed{ 0 };

	for (auto const &[key, action] : movementControls)
	{
		using enum KeyState;
		switch (app->input->GetKey(key))
		{
		case KEY_IDLE:
			actionsPressed |= (action << 3);
			break;
		case KEY_UP:
			actionsPressed |= (action << 2);
			break;
		case KEY_DOWN:
			actionsPressed |= (action << 1);
			break;
		case KEY_REPEAT:
			actionsPressed |= action;
			break;
		}
	}

	return actionsPressed;
}

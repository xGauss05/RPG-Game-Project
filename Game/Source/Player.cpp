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
	return currentMovementVector.IsZero();
}

void Player::Draw() const
{
	DrawParameters drawPlayer(textureID, drawPosition);
	app->render->DrawTexture(drawPlayer.Section(&currentSpriteSlice));
}

void Player::Create()
{
	Sprite::Initialize(0);
	textureID = app->tex->Load("Assets/Textures/Main_Characters/Antonio/Antonio.png");

	if(position.IsZero())
		position = { 192, 2304 };
	//position = { 144, 528 };

	size = { 48, 96 };

	drawPosition = position;
	drawPosition.y -= tileSize;

	currentSpriteSlice = {
		(GetTextureIndex().x + 1) * size.x,
		GetTextureIndex().y * size.y,
		size.x,
		size.y
	};

	actionControls.emplace(SDL_SCANCODE_E, INTERACT_REPEAT);
	actionControls.emplace(SDL_SCANCODE_SPACE, INTERACT_REPEAT);

	movementControls.emplace(SDL_SCANCODE_W, FORWARD_REPEAT);
	movementControls.emplace(SDL_SCANCODE_S, BACKWARD_REPEAT);
	movementControls.emplace(SDL_SCANCODE_A, LEFT_REPEAT);
	movementControls.emplace(SDL_SCANCODE_D, RIGHT_REPEAT);

	directionMeaning.try_emplace(UP, 0, -1);
	directionMeaning.try_emplace(DOWN, 0, 1);
	directionMeaning.try_emplace(LEFT, -1, 0);
	directionMeaning.try_emplace(RIGHT, 1, 0);
}

Player::PlayerAction Player::HandleInput()
{
	using enum KeyState;
	using enum PlayerMoveControlsToBind;

	if (uint16_t actionKeysFlags = GetActionKeysPressed();
		actionKeysFlags & INTERACT_DOWN)
	{
		return PlayerAction::INTERACT;
	}

	auto IsNotZero = [](uint16_t num) -> bool { return ((num | (~num + 1)) >> (sizeof(num) * CHAR_BIT - 1)) & 1; };
	auto IsZero = [IsNotZero](uint16_t num) { return !IsNotZero(num); };

	// Check keys pressed and add/remove them to the collection
	// If it's a new input (aka Key_Down), put it on top of the direction stack
	for (uint16_t movementKeysFlags = GetMovementKeysPressed(), repeatHelper = FORWARD_REPEAT;
		!IsZero(movementKeysFlags) && !IsZero(repeatHelper);
		movementKeysFlags ^= repeatHelper, repeatHelper <<= 1)
	{
		uint16_t pressedKeys = repeatHelper & movementKeysFlags;

		if (IsZero(pressedKeys))
		{
			continue;
		}
		if (pressedKeys & ANY_KEY_REPEAT)
		{
			movementKeysPressed.insert(CastToEnum(ANY_KEY_REPEAT & repeatHelper));
		}
		else if (pressedKeys & ANY_KEY_DOWN)
		{
			directionQueue.push(static_cast<Direction>((ANY_KEY_DOWN & repeatHelper) >> 1));
			movementKeysPressed.insert(CastToEnum((ANY_KEY_DOWN & repeatHelper) >> 1));
		}
		else if (pressedKeys & ANY_KEY_UP)
		{
			movementKeysPressed.erase(CastToEnum((ANY_KEY_UP & repeatHelper) >> 2));
		}
		else if (pressedKeys & ANY_KEY_IDLE)
		{
			movementKeysPressed.erase(CastToEnum((ANY_KEY_IDLE & repeatHelper) >> 3));
		}
	}

	// Check if top of the direction stack is still being pressed, if it isn't remove it and check again
	while (!directionQueue.empty() && !movementKeysPressed.contains(CastToEnum(directionQueue.top())))
	{
		directionQueue.pop();
	}

	// If there's a movement key being pressed
	if(!directionQueue.empty() && currentMovementVector.IsZero())
	{
		lastDirection = directionQueue.top();
		return PlayerAction::MOVE;
	}

	return PlayerAction::NONE;
}

void Player::StartOrRotateMovement(bool walkable)
{
	SelectDirectionTexture();
	
	if (walkable)
		StartMovement();
}

void Player::StartMovement()
{
	lastDirection = directionQueue.top();
	currentMovementVector = directionMeaning[directionQueue.top()] * speed;
	distanceToNextTile = tileSize;
}

void Player::SelectDirectionTexture(Direction direction)
{
	Direction directionToAnim = direction == NONE ? directionQueue.top() : direction;
	switch (directionToAnim)
	{
	case Direction::UP:
		currentSpriteSlice.y = (GetTextureIndex().y + 3) * size.y;
		break;
	case Direction::DOWN:
		currentSpriteSlice.y = GetTextureIndex().y * size.y;
		break;
	case Direction::LEFT:
		currentSpriteSlice.y = (GetTextureIndex().y + 1) * size.y;
		break;
	case Direction::RIGHT:
		currentSpriteSlice.y = (GetTextureIndex().y + 2) * size.y;
		break;
	default:
		break;
	}
}

iPoint Player::GetTileInFront() const
{
	return position + (directionMeaning.at(lastDirection) * tileSize);
}

bool Player::MovedToNewTileThisFrame() const
{
	return bMovedToNewTile;
}

void Player::NewTileChecksDone()
{
	bMovedToNewTile = false;
}

Player::PlayerMoveControlsToBind Player::CastToEnum(uint16_t value) const
{
	return static_cast<PlayerMoveControlsToBind>(value);
}

void Player::Update()
{
	if (!currentMovementVector.IsZero())
	{
		AnimateMovement();
		SmoothMovement();
		app->render->AdjustCamera(position);
	}
	else
	{
		currentSpriteSlice.x = (GetTextureIndex().x + 1) * size.x;

		SelectDirectionTexture(lastDirection);
	}
}

void Player::AnimateMovement()
{
	if (animTimer == 0)
	{
		currentSpriteSlice.x += (size.x * animIncrease);

		if (currentSpriteSlice.x == size.x * (GetTextureIndex().x + 2) || (currentSpriteSlice.x <= size.x * GetTextureIndex().x))
		{
			animIncrease *= -1;
		}

		animTimer = tileSize / speed;
	}
	else
	{
		animTimer--;
	}
}

void Player::SmoothMovement()
{
	if (distanceToNextTile == 0) [[unlikely]]
	{
		currentMovementVector.SetToZero();

		bMovedToNewTile = true;
	}
	else if (distanceToNextTile < 0) [[unlikely]]
	{
		position.FloorToNearest(tileSize);
		distanceToNextTile = 0;
	}
	else [[likely]]
	{
		distanceToNextTile -= abs(currentMovementVector.x + currentMovementVector.y);
		position += currentMovementVector;
		drawPosition += currentMovementVector;
	}
}

uint16_t Player::GetMovementKeysPressed() const
{
	uint16_t actionsPressed{ 0 };

	for (auto const &[key, action] : movementControls)
	{
		actionsPressed |= GetKeyState(key, action);
	}

	return actionsPressed;
}

uint16_t Player::GetActionKeysPressed() const
{
	uint16_t actionsPressed{ 0 };

	for(auto const &[key, action] : actionControls)
	{
		actionsPressed |= GetKeyState(key, action);
	}

	return actionsPressed;
}

uint16_t Player::GetKeyState(SDL_Scancode key, uint16_t action) const
{
	using enum KeyState;
	switch (app->input->GetKey(key))
	{
	case KEY_IDLE:
		return static_cast<uint16_t>(action << 3);
		break;
	case KEY_UP:
		return static_cast<uint16_t>(action << 2);
		break;
	case KEY_DOWN:
		return static_cast<uint16_t>(action << 1);
		break;
	case KEY_REPEAT:
		return action;
		break;
	}

	return 0;
}

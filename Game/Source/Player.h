#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Sprite.h"
#include "Transform.h"
#include "Point.h"

#include <map>
#include <utility>
#include <list>
#include <queue>
#include <stack>
#include <algorithm>
#include <chrono>


class Player : public Sprite, public Transform
{
public:
	enum class PlayerAction
	{
		NONE = 0x0000,
		MOVE = 0x0001,
		INTERACT = 0x0002
	};

	Player();
	~Player() override;
	void Create();

	void Draw() const;

	PlayerAction HandleInput();
	void StartOrRotateMovement(bool walkable = true);
	
	void Update();

	void SetSpeed(int speed);
	void SetPosition(iPoint newPosition);
	iPoint GetDrawPosition() const;

	bool IsStandingStill() const;

	iPoint GetTileInFront() const;

	bool MovedToNewTileThisFrame() const;
	void NewTileChecksDone();

private:

	enum PlayerActionControlsToBind : uint16_t
	{
		INTERACT_REPEAT = 0X0001,
		INTERACT_DOWN = 0X0002,
		INTERACT_UP = 0X0004,
		INTERACT_IDLE = 0X0008,
	};

	std::unordered_multimap<SDL_Scancode, PlayerActionControlsToBind> actionControls;

	enum PlayerMoveControlsToBind : uint16_t
	{
		FORWARD_REPEAT		= 0x0001,
		FORWARD_DOWN		= 0x0002,
		FORWARD_UP			= 0x0004,
		FORWARD_IDLE		= 0x0008,
		BACKWARD_REPEAT		= 0x0010,
		BACKWARD_DOWN		= 0x0020,
		BACKWARD_UP			= 0x0040,
		BACKWARD_IDLE		= 0x0080,
		LEFT_REPEAT			= 0x0100,
		LEFT_DOWN			= 0x0200,
		LEFT_UP				= 0x0400,
		LEFT_IDLE			= 0x0800,
		RIGHT_REPEAT		= 0x1000,
		RIGHT_DOWN			= 0x2000,
		RIGHT_UP			= 0x4000,
		RIGHT_IDLE			= 0x8000,
		ANY_KEY_REPEAT		= 0x1111,
		ANY_KEY_DOWN		= 0x2222,
		ANY_KEY_UP			= 0x4444,
		ANY_KEY_IDLE		= 0x8888
	};

	enum Direction : uint16_t
	{
		NONE	= 0x0000,
		UP		= PlayerMoveControlsToBind::FORWARD_REPEAT,
		DOWN	= PlayerMoveControlsToBind::BACKWARD_REPEAT,
		LEFT	= PlayerMoveControlsToBind::LEFT_REPEAT,
		RIGHT	= PlayerMoveControlsToBind::RIGHT_REPEAT,
		LAST	= 0x0004
	};

	void StartMovement();
	void AnimateMovement();
	void SmoothMovement();

	void SelectDirectionTexture(Direction direction = NONE);
	uint16_t GetMovementKeysPressed() const;

	uint16_t GetKeyState(SDL_Scancode key, uint16_t action) const;
	uint16_t GetKeyState(SDL_GameControllerButton key, uint16_t action) const;

	uint16_t GetActionKeysPressed() const;

	PlayerMoveControlsToBind CastToEnum(uint16_t value) const;

	std::unordered_map<Direction, iPoint> directionMeaning;
	std::unordered_multimap<SDL_Scancode, PlayerMoveControlsToBind> movementControls;
	std::set<PlayerMoveControlsToBind> movementKeysPressed;
	std::stack<Direction> directionQueue;
	
	bool bMovedToNewTile = false;
	int distanceToNextTile = 0;
	Direction lastDirection = DOWN;

	iPoint drawPosition = { 0, 0 };
	SDL_Rect currentSpriteSlice{ 0 };

	iPoint currentMovementVector{ 0 };
	int speed = 8;
	const int tileSize = 48;

	int textureID = -1;
	int animIncrease = 1;
	int animTimer = 0;

};

#endif //__PLAYER_H__


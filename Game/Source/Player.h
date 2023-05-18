#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Sprite.h"
#include "Transform.h"
#include "Point.h"

#include <map>
#include <utility>
#include <list>
#include <queue>
#include <algorithm>
#include <chrono>


class Player : public Sprite, public Transform
{
public:
	

	struct PlayerAction
	{
		enum class Action
		{
			NONE = 0x0000,
			MOVE = 0x0001,
			INTERACT = 0x0002
		};

		friend Action operator&(Action a, Action b)
		{
			return static_cast<Action>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
		}

		friend Action operator&=(Action& a, Action b)
		{
			return a = a & b;
		}

		friend Action operator|(Action a, Action b)
		{
			return static_cast<Action>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
		}

		friend Action &operator|=(Action& a, Action b)
		{
			return a = a | b;
		}


		iPoint destinationTile = { 0, 0 };

		Action action = Player::PlayerAction::Action::NONE;
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

	void RotatePlayer();
	bool IsStandingStill() const;

	void SetMovementStopped(bool b);
	bool GetMovementStopped() const;

	iPoint lastDir{ 0 };

private:
	void AnimateMove();
	void SmoothMove();
	uint16_t GetActionsKeysPressed() const;
	void StartMovement();

	iPoint drawPosition = { 0, 0 };


	int moveTimer = 0;
	iPoint moveVector{ 0 };
	int speed = 16;
	const int timeForATile = 2;
	const int tileSize = 48;
	int animIncrease = 1;

	int animTimer = 0;

	bool movementStopped = false;

	SDL_Rect currentSpriteSlice{ 0 };

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
		UP		= PlayerMoveControlsToBind::FORWARD_REPEAT,
		DOWN	= PlayerMoveControlsToBind::BACKWARD_REPEAT,
		LEFT	= PlayerMoveControlsToBind::LEFT_REPEAT,
		RIGHT	= PlayerMoveControlsToBind::RIGHT_REPEAT,
		LAST	= 0x0004
	};

	enum PlayerActionControlsToBind : uint16_t
	{
		INTERACT_REPEAT = 0X0001,
		INTERACT_DOWN = 0X0002,
		INTERACT_UP = 0X0004,
		INTERACT_IDLE = 0X0008,
	};

	Direction direction = Direction::DOWN;

	std::unordered_multimap<SDL_Scancode, PlayerMoveControlsToBind> movementControls;
	std::unordered_multimap<SDL_Scancode, PlayerActionControlsToBind> actionControls;


	using Time = std::chrono::steady_clock;
	using Milliseconds = std::chrono::milliseconds;
	using FloatSeconds = std::chrono::duration<float>;
	using StdTimePoint = std::chrono::time_point<Time>;
	using ActionQueueInfo = std::pair<StdTimePoint, PlayerMoveControlsToBind>;
	

	struct ActionQueueComparator
	{
		constexpr bool operator()(ActionQueueInfo const& a, ActionQueueInfo const& b) const noexcept
		{
			return std::chrono::duration_cast<Milliseconds>(a.first - b.first).count() > 0 ? true : false;
		}
	};

	using ActionPriorityQueue = std::priority_queue<ActionQueueInfo, std::set<ActionQueueInfo>, ActionQueueComparator>;

	//ActionPriorityQueue actionQueue;

	std::set<PlayerMoveControlsToBind> actionQueue;

	PlayerMoveControlsToBind CastToEnum(uint16_t value) const;
};

#endif //__PLAYER_H__


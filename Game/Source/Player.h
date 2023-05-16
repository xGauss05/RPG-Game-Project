#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Sprite.h"
#include "Transform.h"
#include "Point.h"


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
	~Player();
	void Create();

	void Draw() const;

	PlayerAction HandleInput() const;
	void StartMovementIfAble(bool walkable = true);
	
	void Update();

	void SetSpeed(int speed);
	void SetPosition(iPoint newPosition);
	iPoint GetPosition() const;

	void RotatePlayer();
	bool IsStandingStill() const;

	void SetMovementStopped(bool b);
	bool GetMovementStopped() const;

	iPoint lastDir{ 0 };

private:
	void AnimateMove();
	void SmoothMove();
	void StartMovement();


	int moveTimer = 0;
	iPoint moveVector{ 0 };
	int speed = 16;
	const int timeForATile = 2;
	const int tileSize = 48;

	int animTimer = 0;

	bool movementStopped = false;

	SDL_Rect currentSpriteSlice{ 0 };
};

#endif //__PLAYER_H__


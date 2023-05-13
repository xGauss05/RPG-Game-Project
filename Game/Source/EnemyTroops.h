#ifndef __GAME_ENEMY_H__
#define __GAME_ENEMY_H__

#include "Point.h"

#include <vector>

#include "PugiXml/src/pugixml.hpp"

struct Enemy
{
	std::string name;
	int textureID;
	int deadSfx;
	int currentHP;
	int currentMana;
	int currentXP;

	int level;

	std::vector<int> stats;
	std::vector<int> equipment;
	bool isDefending = false;
	std::vector<int> skills;

	iPoint position{};
	iPoint size{};

	int index;

	Uint8 alpha = 255;
	bool fadingIn = false;

	bool IsMouseHovering() const;
};

class EnemyTroops
{
public:
	EnemyTroops() = default;

	void CreateFight(std::string_view nodeName);
	

	std::vector<Enemy> troop;
};

#endif //__GAME_ENEMY_H__


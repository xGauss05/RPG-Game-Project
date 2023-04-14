#ifndef __GAME_ENEMY_H__
#define __GAME_ENEMY_H__

#include <vector>

struct Enemy
{
	int textureID;
	int currentHP;
	int currentMana;
	int currentXP;

	int level;

	std::vector<int> stats;
	std::vector<int> equipment;

	std::vector<int> skills;
};

class EnemyTroops
{
public:
	EnemyTroops() = default;

	std::vector<Enemy> troop;
};

#endif //__GAME_ENEMY_H__


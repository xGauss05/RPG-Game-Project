#ifndef __GAME_ENEMY_H__
#define __GAME_ENEMY_H__

#include <vector>

#include "PugiXml/src/pugixml.hpp"

struct Enemy
{
	std::string name;
	int textureID;
	int currentHP;
	int currentMana;
	int currentXP;

	int level;

	std::vector<int> stats;
	std::vector<int> equipment;
	bool isDefending = false;
	std::vector<int> skills;
};

class EnemyTroops
{
public:
	EnemyTroops() = default;

	void CreateFight(std::string const &nodeName);

	std::vector<Enemy> troop;
};

#endif //__GAME_ENEMY_H__


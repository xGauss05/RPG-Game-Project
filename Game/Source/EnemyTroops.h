#ifndef __GAME_ENEMY_H__
#define __GAME_ENEMY_H__

#include "Point.h"

#include <vector>

#include "PugiXml/src/pugixml.hpp"

enum class BaseStats
{
	MAX_HP = 0,
	MAX_MANA,
	ATTACK,
	DEFENSE,
	SPECIAL_ATTACK,
	SPECIAL_DEFENSE,
	SPEED,
	LEVEL,
	XP
};

struct Battler
{
	std::string name = "";
	int battlerTextureID = 0;
	int portraitTextureID = -1;
	int deadSfx = 0;
	int currentHP = 0;
	int currentMana = 0;
	int level = 0;

	bool isDefending = false;

	iPoint position{};
	iPoint size{};

	int index;

	Uint8 alpha = 255;
	int fadingDirection = 1;

	bool IsMouseHovering() const;

	bool IsDead() const;

	int GetStat(BaseStats stat) const;

	void SetCurrentHP(int hp);
	void SetCurrentMana(int mp);
	void SetCurrentXP(int xp);
	void SetLevel(int lvl);

	// Returns new Level if level up. -1 if not.
	int AddXP(int amount);

	bool UseItem(Item const& item);

	int GetCurrentXP() const;
	int GetXPToNextLevel() const;

	std::string GetStatDisplay(BaseStats stat, bool choosingChar = false) const;

	bool RestoreHP(float amount1, float amount2);
	
	void AddStat(int value);

private:
	int currentXP = 0;
	std::vector<int> stats;
	std::vector<int> equipment;
	std::vector<int> skills;
};

class EnemyTroops
{
public:
	EnemyTroops() = default;

	void CreateFight(std::string_view nodeName);
	

	std::vector<Battler> troop;
};

#endif //__GAME_ENEMY_H__


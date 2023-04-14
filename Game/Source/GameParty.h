#ifndef __GAME_PARTY_H__
#define __GAME_PARTY_H__

#include <vector>

enum class BaseStats
{
	MAX_HP = 0,
	MAX_MANA,
	ATTACK,
	DEFENSE,
	SPECIAL_ATTACK,
	SPECIAL_DEFENSE,
	SPEED
};

enum class EquipmentSlots
{
	HEAD,
	ARMOR,
	ACCESSORY,
	WEAPON
};

struct PartyCharacter
{
	std::string name;
	int battlerTextureID;
	int currentHP;
	int currentMana;
	int currentXP;

	int level;

	std::vector<int> stats;
	bool isDefending = false;
	std::vector<int> equipment;
	std::vector<int> skills;
};

class GameParty
{
public:
	GameParty();
	void CreateParty();
	std::vector<PartyCharacter> party;
	std::vector<int> inventory;
};

#endif //__GAME_PARTY_H__


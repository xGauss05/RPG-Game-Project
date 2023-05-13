#ifndef __GAME_PARTY_H__
#define __GAME_PARTY_H__

#include <vector>

#include "DB_Items.h"

#include "Log.h"

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

enum class EquipmentSlots
{
	ARMOR,
	ACCESSORY,
	WEAPON
};

struct PartyCharacter
{
	std::string name = "";
	int battlerTextureID = 0;
	int currentHP = 0;
	int currentMana = 0;
	int currentXP = 0;

	int level = 0;

	std::vector<int> stats;
	bool isDefending = false;
	std::vector<int> equipment;
	std::vector<int> skills;

	void SetCurrentHP(int hp);
	void SetCurrentMana(int mp);
	void SetCurrentXP(int xp);
	void SetLevel(int lvl);

	bool UseItem(Item const &item);

	std::string GetStatDisplay(BaseStats stat) const;

	bool RestoreHP(float amount1, float amount2);
};

class GameParty
{
public:
	GameParty();
	void CreateParty();

	void AddItemToInventory(std::string_view itemToAdd, int amountToAdd = 1);
	void AddItemToInventory(int  itemToAdd, int amountToAdd = 1);

	void RemoveItemFromInventory(std::string_view itemToRemove, int amountToRemove = 1);
	void RemoveItemFromInventory(int itemToRemove, int amountToRemove = 1);
	void RemoveItemFromInventory(std::vector<std::pair<int, int>>::iterator it, int amountToRemove = 1);

	void UseItemOnMap(int character, int itemId, int amountToUse = 1);

	std::vector<PartyCharacter> party;
	std::vector<std::pair<int, int>> inventory;

	std::unique_ptr<DB_Items> dbItems;
};

#endif //__GAME_PARTY_H__


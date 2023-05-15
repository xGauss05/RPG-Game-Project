#ifndef __GAME_PARTY_H__
#define __GAME_PARTY_H__

#include "DB_Items.h"
#include "DB_Quests.h"

#include "Log.h"

#include <vector>
#include <map>
#include <unordered_map>


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

	std::unordered_map<int, bool> globalSwitch;

	void AddItemToInventory(std::string_view itemToAdd, int amountToAdd = 1);
	void AddItemToInventory(int  itemToAdd, int amountToAdd = 1);

	void RemoveItemFromInventory(std::string_view itemToRemove, int amountToRemove = 1);
	void RemoveItemFromInventory(int itemToRemove, int amountToRemove = 1);
	void RemoveItemFromInventory(std::vector<std::pair<int, int>>::iterator it, int amountToRemove = 1);

	void AddGold(int amount);
	void RemoveGold(int amount);
	int GetGold() const;
	void SetGold(int amount);

	void UseItemOnMap(int character, int itemId, int amountToUse = 1);

	void AcceptQuest(int id);
	void CompleteQuest(int id);
	bool IsQuestAvailable(int id) const;
	void PossibleQuestProgress(
		QuestType t,
		std::vector<std::pair<std::string_view, int>> const& names,
		std::vector<std::pair<int, int>> const& IDs
	);
	bool GetUpdateQuestLog();
	std::string QuestCompleteMessage();
	bool IsQuestMessagePending() const;

	std::vector<PartyCharacter> party;
	std::vector<std::pair<int, int>> inventory;

	int currentGold = 0;

	std::unique_ptr<DB_Items> dbItems;
	std::unique_ptr<DB_Quests> dbQuests;

	std::unordered_map<int, bool> globalSwitches;

	bool updateQuestLog = false;
	Quest *lastQuestCompleted;

	std::unordered_map<int, std::unique_ptr<Quest>> currentQuests;

	// Questype -> All indexes of accepted Quests that have that Questype
	std::unordered_map<QuestType, std::vector<int>> currentQuestsCategories;

	std::unordered_map<int, std::unique_ptr<Quest>> completedQuests;

	friend class Map_Menu_Inventory;
};

#endif //__GAME_PARTY_H__


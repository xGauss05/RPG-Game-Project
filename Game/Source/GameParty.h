#ifndef __GAME_PARTY_H__
#define __GAME_PARTY_H__

#include "DB_Items.h"
#include "DB_Quests.h"

#include "ObserverPattern.h"

#include "EnemyTroops.h"

#include "Log.h"

#include <vector>
#include <map>
#include <unordered_map>


enum class EquipmentSlots
{
	ARMOR,
	ACCESSORY,
	WEAPON
};

struct PartyCharacter : public Battler
{
	
};

class GameParty : public Publisher
{
public:
	GameParty();
	void CreateParty();

	void AddItemToInventory(std::string_view itemToAdd, int amountToAdd = 1);
	void AddItemToInventory(int  itemToAdd, int amountToAdd = 1);

	bool HasItemInInventory(int itemID) const;

	void RemoveItemIndexFromInventory(int itemIndex, int amountToRemove = 1);

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


	// Returns a pair consisting of an state of the inserted element,
	// or the already-existing element if no insertion happened,
	// and a bool denoting whether the insertion took place 
	// (true if insertion happened, false if it did not).
	std::pair<bool, bool>AddGlobalSwitch(int id, bool state = false);

	// Returns new state
	bool ToggleGlobalSwitchState(int id);

	bool GlobalSwitchExists(int id) const;

	// Inserts a new element in case it doesn't exist. 
	// Doesn't initialize bool value.
	bool GetGlobalSwitchState(int id);

	// Inserts a new element in case it doesn't exist. 
	void SetGlobalSwitchState(int id, bool newState);

	void Attach(ISubscriber* sub, int id) final;

	void Notify(int id, bool state) final;

	std::unordered_map<int, bool> globalSwitches;

	bool updateQuestLog = false;
	Quest *lastQuestCompleted = nullptr;

	std::vector<Battler> party;
	std::vector<std::pair<int, int>> inventory;

	int currentGold = 0;

	std::unique_ptr<DB_Items> dbItems;
	std::unique_ptr<DB_Quests> dbQuests;

	// Questype -> All indexes of accepted Quests that have that Questype
	std::unordered_map<QuestType, std::vector<int>> currentQuestsCategories;


	std::unordered_map<int, std::unique_ptr<Quest>> currentQuests;
	std::unordered_map<int, std::unique_ptr<Quest>> completedQuests;

	friend class Map_Menu_Inventory;
};

#endif //__GAME_PARTY_H__


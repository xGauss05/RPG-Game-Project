#include "App.h"
#include "GameParty.h"
#include "TextureManager.h"
#include "Log.h"
#include "PugiXml/src/pugixml.hpp"

void PartyCharacter::SetCurrentHP(int hp) {
	currentHP = hp;
}
void PartyCharacter::SetCurrentMana(int mp) {
	currentMana = mp;
}
void PartyCharacter::SetCurrentXP(int xp) {
	currentXP = xp;
}
void PartyCharacter::SetLevel(int lvl) {
	level = lvl;
}

bool PartyCharacter::UseItem(Item const& item)
{
	switch (item.effect.functionToCall)
	{
	case 11:
		return RestoreHP(item.effect.param1, item.effect.param2);
		break;
	default:
		LOG("Function effect %i not added", item.effect.functionToCall);
		return false;
	}
}

bool PartyCharacter::RestoreHP(float amount1, float amount2)
{
	int maxHP = stats[static_cast<int>(BaseStats::MAX_HP)];

	if (currentHP <= 0 || currentHP >= maxHP)
		return false;

	auto GetAmountToAdd = [maxHP](float n)
		{
			if (ceilf(n) == n)
				return static_cast<int>(n);
			else
				return static_cast<int>(n * static_cast<float>(maxHP));
		};

	currentHP += GetAmountToAdd(amount1) + GetAmountToAdd(amount2);

	if (currentHP > maxHP)
	{
		currentHP = maxHP;
	}

	return true;
}

GameParty::GameParty() = default;

void GameParty::CreateParty()
{
	pugi::xml_document battlersFile;
	if (auto result = battlersFile.load_file("data/Characters.xml"); !result)
	{
		LOG("Could not load battlers xml file. pugi error: %s", result.description());
	}
	for (auto const& character : battlersFile.children("character"))
	{
		PartyCharacter memberToAdd;
		memberToAdd.name = character.child("general").attribute("name").as_string();
		memberToAdd.level = character.child("general").attribute("level").as_int();
		memberToAdd.battlerTextureID = app->tex->Load(character.child("texture").attribute("path").as_string());
		for (auto const& stat : character.child("stats").children())
		{
			memberToAdd.stats.emplace_back(stat.attribute("value").as_int());
		}
		memberToAdd.currentHP = memberToAdd.stats[0];
		memberToAdd.currentMana = memberToAdd.stats[1];
		party.emplace_back(memberToAdd);
	}

	dbItems = std::make_unique<DB_Items>();
	dbQuests = std::make_unique<DB_Quests>();
}

void GameParty::UseItemOnMap(int character, int itemId, int amountToUse)
{
	for (auto it = inventory.begin(); it != inventory.end(); ++it)
	{
		// If the item is in the inventory
		if (it->first == itemId)
		{
			// If the item was able to be used succesfully
			if (bool success = party[character].UseItem(dbItems->GetItem(itemId));
				success)
			{
				RemoveItemFromInventory(it, amountToUse);

				return;
			}
		}
	}
}

void GameParty::AcceptQuest(int id)
{
	if (auto [it, success, nodeHandle] = currentQuests.insert(dbQuests->QuestAccepted(id));
		success)
	{
		auto objectivesSet = it->second->GetQuestTypeSet();
		for(auto const &elem : objectivesSet)
		{
			currentQuestsCategories[elem].emplace_back(id);
		}
		updateQuestLog = true;
	}
}

void GameParty::CompleteQuest(int id)
{
	if (auto [it, success, nodeHandle] = completedQuests.insert(currentQuests.extract(id));
		success)
	{
		lastQuestCompleted = (it->second).get();

		auto objectivesSet = it->second->GetQuestTypeSet();

		for (auto const& elem : objectivesSet)
		{
			std::erase_if(currentQuestsCategories[elem], [id](int index) { return index == id; });
		}
	}
}

bool GameParty::IsQuestAvailable(int id) const
{
	return !(currentQuests.contains(id) || completedQuests.contains(id));
}

void GameParty::PossibleQuestProgress(QuestType t, std::vector<std::pair<std::string_view, int>> const& names, std::vector<std::pair<int, int>> const& IDs)
{
	std::vector<int> const& questsToCheck = currentQuestsCategories[t];
	for (int questIndex : questsToCheck)
	{
		if (auto it = currentQuests.find(questIndex);
			it != currentQuests.end())
		{
			it->second->ProgressQuest(t, names, IDs);

			if (it->second->IsQuestCompleted())
			{
				CompleteQuest(questIndex);
			}

			updateQuestLog = true;
		}
		else
		{
			std::erase_if(currentQuestsCategories[t], [questIndex](int i) { return questIndex == i; });
		}
	}
}

bool GameParty::GetUpdateQuestLog()
{
	bool retValue = updateQuestLog;
	updateQuestLog = false;

	return retValue;
}

bool GameParty::IsQuestMessagePending() const
{
	return lastQuestCompleted;
}

std::string GameParty::QuestCompleteMessage()
{
	std::string_view questName = lastQuestCompleted->GetQuestName();

	lastQuestCompleted = nullptr;

	return std::format("Quest \"{}\" completed.", questName);
}

void GameParty::AddItemToInventory(std::string_view itemToAdd, int amountToAdd)
{
	int itemToAddID = dbItems->GetItemID(itemToAdd);

	AddItemToInventory(itemToAddID, amountToAdd);
}

void GameParty::AddItemToInventory(int itemToAdd, int amountToAdd)
{
	if (itemToAdd == 0)
		return;

	bool alreadyInInventory = false;

	for (auto& [itemID, amount] : inventory)
	{
		if (itemID == itemToAdd)
		{
			amount += amountToAdd;
			alreadyInInventory = true;
			break;
		}
	}

	if (!alreadyInInventory)
	{
		inventory.emplace_back(itemToAdd, amountToAdd);
	}

	std::vector<std::pair<int, int>> itemAddedToInventory;
	itemAddedToInventory.emplace_back(itemToAdd, amountToAdd);

	PossibleQuestProgress(QuestType::COLLECT, std::vector<std::pair<std::string_view, int>>(), itemAddedToInventory);
}

void GameParty::RemoveItemFromInventory(std::string_view itemToRemove, int amountToRemove)
{
	int itemToRemoveID = dbItems->GetItemID(itemToRemove);

	RemoveItemFromInventory(itemToRemoveID, amountToRemove);
}

void GameParty::RemoveItemFromInventory(int itemToRemove, int amountToRemove)
{
	for (auto it = inventory.begin(); it != inventory.end(); ++it)
	{
		if (it->first == itemToRemove)
		{
			RemoveItemFromInventory(it, amountToRemove);
		}
	}
}

void GameParty::RemoveItemFromInventory(std::vector<std::pair<int, int>>::iterator it, int amountToRemove)
{
	it->second -= amountToRemove;
	// If player has no more items of the same id, erase it from the inventory
	if (it->second <= 0)
	{
		inventory.erase(it);
	}
}

void GameParty::AddGold(int amount)
{
	currentGold += amount;
	if (currentGold > 999999)
	{
		currentGold = 999999;
	}
}

void GameParty::RemoveGold(int amount)
{
	currentGold -= amount;
	if (currentGold < 0) [[unlikely]]
	{
		currentGold = 0;
	}
}

int GameParty::GetGold() const
{
	return currentGold;
}

void GameParty::SetGold(int amount)
{
	currentGold = amount;
}

std::string PartyCharacter::GetStatDisplay(BaseStats stat) const
{
	using enum BaseStats;
	switch (stat)
	{
		case MAX_HP:			return std::format("HP: {} / {}", currentHP, stats[0]);
		case MAX_MANA:			return std::format("MP: {} / {}", currentMana, stats[1]);
		case ATTACK:			return std::format("Atk: {}", stats[2]);
		case DEFENSE:			return std::format("Def: {}", stats[3]);
		case SPECIAL_ATTACK: 	return std::format("Sp. Atk: {}", stats[4]);
		case SPECIAL_DEFENSE:	return std::format("Sp. Def: {}", stats[5]);
		case SPEED:				return std::format("Speed: {}", stats[6]);
		case LEVEL:				return std::format("Lv. {}", level);
		case XP:				return std::format("EXP: {}", currentXP);
	}
}

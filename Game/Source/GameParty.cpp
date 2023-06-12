#include "App.h"
#include "Audio.h"
#include "GameParty.h"
#include "TextureManager.h"
#include "Log.h"
#include "PugiXml/src/pugixml.hpp"

void Battler::SetCurrentHP(int hp) {
	currentHP = hp;
}
void Battler::SetCurrentMana(int mp) {
	currentMana = mp;
}
void Battler::SetCurrentXP(int xp) {
	currentXP = xp;
}
void Battler::SetLevel(int lvl) {
	level = lvl;
}

int Battler::AddXP(int amount)
{
	currentXP += amount;
	if (int xp_needed = GetXPToNextLevel();
		currentXP >= xp_needed && level <= 99)
	{
		currentXP -= xp_needed;
		currentHP += 3;
		currentMana += 3;
		level++;
		return level;
	}
	return -1;
}

bool Battler::UseItem(Item const& item)
{
	switch (item.effect.functionToCall)
	{
	case 11:
	{
		int restoredHP = RestoreHP(item.effect.param1, item.effect.param2);

		if (restoredHP <= 0)
		{
			itemTextToDisplay = std::format("It has no effect...");
		}
		else
		{
			if (item.effect.text.size() >= 2)
			{
				itemTextToDisplay = GetStat(BaseStats::MAX_HP) > currentHP ?
					AddSaveData(item.effect.text[0], name, restoredHP) :
					AddSaveData(item.effect.text[1], name);
			}
			else
			{
				itemTextToDisplay = GetStat(BaseStats::MAX_HP) > currentHP ?
					std::format("{} recovers {} HP!", name, restoredHP) :
					std::format("{}'s HP is fully recovered!", name);
			}

			if (!item.general.sfxPath.empty())
			{
				app->audio->PlayFx(item.general.sfxID);
			}
		}

		return to_bool(restoredHP);
	}
	case 12:
	{
		int restoredMP = RestoreMP(item.effect.param1, item.effect.param2);

		if (restoredMP <= 0)
		{
			itemTextToDisplay = std::format("It has no effect...");
		}
		else
		{
			if (item.effect.text.size() >= 2)
			{
				itemTextToDisplay = GetStat(BaseStats::MAX_MANA) > currentMana ?
					AddSaveData(item.effect.text[0], name, restoredMP) :
					AddSaveData(item.effect.text[1], name);
			}
			else
			{
				itemTextToDisplay = GetStat(BaseStats::MAX_MANA) > currentMana ?
					std::format("{} recovers {} Mana!", name, restoredMP) :
					std::format("{}'s Mana is fully recovered!", name);
			}
			if (!item.general.sfxPath.empty())
			{
				app->audio->PlayFx(item.general.sfxID);
			}
		}

		return to_bool(restoredMP);
	}
	case 13:
	{
		int revivedHP = Revive(item.effect.param1, item.effect.param2);

		if (revivedHP <= 0)
		{
			itemTextToDisplay = std::format("It has no effect...");
		}
		else
		{
			if (item.effect.text.size() >= 2)
			{
				itemTextToDisplay = GetStat(BaseStats::MAX_HP) > currentHP ?
					AddSaveData(item.effect.text[0], name, revivedHP) :
					AddSaveData(item.effect.text[1], name);
			}
			else
			{
				itemTextToDisplay = GetStat(BaseStats::MAX_HP) > currentHP ?
					std::format("{} revived for {} HP!", name, revivedHP) :
					std::format("{} is fully revived!", name);
			}

			if (!item.general.sfxPath.empty())
			{
				app->audio->PlayFx(item.general.sfxID);
			}
		}

		return to_bool(revivedHP);
	}
	default:
		LOG("Function effect %i not added", item.effect.functionToCall);
		return false;
	}
}

int Battler::GetCurrentXP() const
{
	return currentXP;
}

int Battler::GetXPToNextLevel() const
{
	return static_cast<int>(std::round(100 + (0.75f * std::pow(level, 3))));
}

int Battler::RestoreHP(float amount1, float amount2)
{
	int maxHP = GetStat(BaseStats::MAX_HP);
	int HPBeforeHealing = currentHP;

	if (currentHP <= 0 || currentHP >= maxHP)
		return 0;

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

	int HPHealed = currentHP - HPBeforeHealing;

	return HPHealed;
}

void Battler::SetParticleEmitters(ParticleDB const &db)
{
	if(!AreEmittersSet())
	{
		particles.AddEmitter(db, ParticleDB::BluePrintTypes::FIRE, position);
	}
}

int Battler::RestoreMP(float amount1, float amount2)
{
	int maxMP = GetStat(BaseStats::MAX_MANA);
	int MPBeforeHealing = currentMana;

	if (currentMana <= 0 || currentMana >= maxMP)
		return 0;

	auto GetAmountToAdd = [maxMP](float n)
	{
		if (ceilf(n) == n)
			return static_cast<int>(n);
		else
			return static_cast<int>(n * static_cast<float>(maxMP));
	};

	currentMana += GetAmountToAdd(amount1) + GetAmountToAdd(amount2);

	if (currentMana > maxMP)
	{
		currentMana = maxMP;
	}

	int MPHealed = currentMana - MPBeforeHealing;

	return MPHealed;
}

bool Battler::AreEmittersSet() const
{
	return particles.IsValid();
}

int Battler::Revive(float amount1, float amount2)
{
	int maxHP = GetStat(BaseStats::MAX_HP);

	if (currentHP > 0)
		return 0;

	auto GetAmountToAdd = [maxHP](float n)
	{
		if (ceilf(n) == n)
			return static_cast<int>(n);
		else
			return static_cast<int>(n * static_cast<float>(maxHP));
	};

	currentHP += GetAmountToAdd(amount2);

	if (currentHP > maxHP)
	{
		currentHP = maxHP;
	}

	int HPHealed = currentHP;

	return HPHealed;
}

void Battler::UpdateParticles()
{
	particles.Update();
}

void Battler::AddStat(int value)
{
	stats.emplace_back(value);
}

std::string Battler::GetStatDisplay(BaseStats stat, bool choosingChar) const
{
	using enum BaseStats;
	if (choosingChar)
	{
		if (stat == LEVEL)
			return std::format("Lvl. {}", level);
		else if (stat == MAX_HP)
			return std::format("{} / {}", currentHP, GetStat(MAX_HP));
		else if (stat == BaseStats::MAX_MANA)
			return std::format("{} / {}", currentMana, GetStat(MAX_MANA));
	}

	switch (stat)
	{
	case MAX_HP:			return std::format("HP: {} / {}", currentHP, GetStat(MAX_HP));
	case MAX_MANA:			return std::format("MP: {} / {}", currentMana, GetStat(MAX_MANA));
	case ATTACK:			return std::format("Atk: {}", GetStat(ATTACK));
	case DEFENSE:			return std::format("Def: {}", GetStat(DEFENSE));
	case SPECIAL_ATTACK: 	return std::format("Sp. Atk: {}", GetStat(SPECIAL_ATTACK));
	case SPECIAL_DEFENSE:	return std::format("Sp. Def: {}", GetStat(SPECIAL_DEFENSE));
	case SPEED:				return std::format("Speed: {}", GetStat(SPEED));
	case LEVEL:				return std::format("Lvl. {}", level);
	case XP:				return std::format("EXP: {}", currentXP);
	}
}

std::string Battler::GetTextToDisplay() const
{
	return itemTextToDisplay;
}

bool Battler::IsDead() const
{
	return currentHP <= 0;
}

int Battler::GetStat(BaseStats stat) const
{
	return stats[static_cast<int>(stat)] + (3 * level);
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
		Battler memberToAdd;
		memberToAdd.name = character.child("general").attribute("name").as_string();
		memberToAdd.level = character.child("general").attribute("level").as_int();
		memberToAdd.battlerTextureID = app->tex->Load(character.child("texture").attribute("path").as_string());
		memberToAdd.portraitTextureID = app->tex->Load(character.child("portraittexture").attribute("path").as_string());
		for (auto const& stat : character.child("stats").children())
		{
			memberToAdd.AddStat(stat.attribute("value").as_int());
		}
		memberToAdd.currentHP = memberToAdd.GetStat(BaseStats::MAX_HP);
		memberToAdd.currentMana = memberToAdd.GetStat(BaseStats::MAX_MANA);
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
		for (auto const& elem : objectivesSet)
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

bool GameParty::GlobalSwitchExists(int id) const
{
	if (auto it = globalSwitches.find(id);
		it != globalSwitches.end())
	{
		return true;
	}

	return false;
}

bool GameParty::GetGlobalSwitchState(int id)
{
	return globalSwitches[id];
}

void GameParty::SetGlobalSwitchState(int id, bool newState)
{
	globalSwitches[id] = newState;
	LOG(std::format("Set global Switch {} to {}.", id, newState).c_str());
	Notify(id, globalSwitches[id]);
}

void GameParty::Attach(ISubscriber * sub, int id)
{
	/*if (!GlobalSwitchExists(id))
	{
		SetGlobalSwitchState(id, false);
	}*/
	Publisher::Attach(sub, id);
}

void GameParty::Notify(int id, bool state)
{
	Publisher::Notify(id, state);

	auto& gSToUpdate = GetGlobalSwitchsToUpdate();

	while (!gSToUpdate.empty())
	{
		auto const& [index, gSState] = gSToUpdate.top();
		gSToUpdate.pop();

		SetGlobalSwitchState(index, !gSState);
	}
}

std::pair<bool, bool> GameParty::AddGlobalSwitch(int id, bool state)
{
	auto const& [it, success] = globalSwitches.try_emplace(id, state);
	if (success)
	{
		Notify(id, globalSwitches[id]);
		LOG(std::format("Succesfully added global Switch {} with state {}.", id, state).c_str());
	}
	else
	{
		LOG(std::format("Couldn't add global Switch {}.", id).c_str());
	}

	return std::pair(it->second, success);
}

bool GameParty::ToggleGlobalSwitchState(int id)
{
	Notify(id, !globalSwitches[id]);
	LOG(std::format("Global Switch {} toggled to {}.", id, !globalSwitches[id]).c_str());
	return globalSwitches[id] = !globalSwitches[id];
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

bool GameParty::HasItemInInventory(int itemID) const
{
	return std::ranges::any_of(inventory, [itemID](std::pair<int, int> i) {return i.first == itemID; });
}

void GameParty::RemoveItemIndexFromInventory(int itemIndex, int amountToRemove)
{
	RemoveItemFromInventory(inventory.begin() + itemIndex, amountToRemove);
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
			return;
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

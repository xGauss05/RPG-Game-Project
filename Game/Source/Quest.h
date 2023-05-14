#ifndef __QUEST_H__
#define __QUEST_H__

#include "App.h"

#include "Log.h"

#include <string>
#include <unordered_set>

enum class QuestType
{
	UNKNOWN			= 0x0000,
	TALK_TO			= 0x0001,
	COLLECT			= 0x0002,
	VISIT			= 0x0004,
	HUNT			= 0x0008,
	CURRENT_QUEST	= 0x0010,
	NEXT_QUEST		= 0x0020,
	NONE			= 0x0040,
	LAST			= 0x0080
};

inline QuestType operator|(QuestType a, QuestType b)
{
	return static_cast<QuestType>(std::byte(a) | std::byte(b));
}
inline QuestType& operator|=(QuestType& a, QuestType b)
{
	return a = a | b;
}

class Quest_Leaf;
class Quest_Branch;

class Quest_Component
{
public:
	virtual ~Quest_Component() = default;
	void SetParent(Quest_Component* p);
	Quest_Component* GetParent() const;

	virtual void ToggleEnabled();
	virtual bool IsEnabled() const;

	virtual void Add(std::unique_ptr<Quest_Component>& component);
	virtual void Add(std::unique_ptr<Quest_Branch>& component);
	virtual void AddLeaf(std::unique_ptr<Quest_Leaf>& component);
	virtual void Remove(Quest_Component* component);
	virtual void Debug() const;
	virtual void ProgressQuest(
		std::vector<std::pair<std::string_view, int>> const& names,
		std::vector<std::pair<int, int>> const& IDs
	);
	virtual QuestType GetType() const;
	virtual void CheckChildrenProgress();


private:
	Quest_Component* parent = nullptr;
	bool enabled = true;
};

class Quest_Leaf : public Quest_Component
{
public:

	void ParseXMLGeneralProperties(pugi::xml_node const& objectiveNode);

	bool CheckCompletion();

	void ProgressQuest(
		std::vector<std::pair<std::string_view, int>> const& names,
		std::vector<std::pair<int, int>> const& IDs
	) override;
	void AddToProgress(int amount = 1);

	std::pair<int, int> GetCurrentProgress() const;
	std::pair<std::string_view, int>  GetObjective() const;

	void Debug() const override;

private:
	std::string objectiveName = "";
	int objectiveID = 0;
	int amountNeeded = 1;
	int amountAcquired = 0;
};

class Quest_Branch : public Quest_Component
{
public:
	void Add(std::unique_ptr<Quest_Component>& component) override;
	void Add(std::unique_ptr<Quest_Branch>& component) override;
	void AddLeaf(std::unique_ptr<Quest_Leaf>& component) override;

	void Remove(Quest_Component* component) override;

	bool HasChildren() const;
	void ProgressQuest(
		std::vector<std::pair<std::string_view, int>> const& names,
		std::vector<std::pair<int, int>> const& IDs
	) override;
	void CheckChildrenProgress() override;

	void SetType(QuestType t);
	QuestType GetType() const override;

	void Debug() const override;

private:
	QuestType type = QuestType::UNKNOWN;
	std::list<std::unique_ptr<Quest_Component>> children;

	friend class Quest_Root;
};

class Quest_Root : public Quest_Branch
{
public:
	void AddNextQuest(std::unique_ptr<Quest_Root>& component);

	void AddLeafToLastBranchAdded(std::unique_ptr<Quest_Leaf>& component);

	void CheckIfObjectiveType(
		QuestType t,
		std::vector<std::pair<std::string_view, int>> const& names,
		std::vector<std::pair<int, int>> const& IDs
	) const;

	void CheckChildrenProgress() override;

	void ToggleQuestCompleted();
	bool IsQuestCompleted() const;

	void Debug() const override;

private:
	bool questCompleted = false;
};

class Quest
{
public:
	Quest() = default;
	explicit Quest(pugi::xml_node const& generalNode);
	~Quest() = default;

	void AddBranch(QuestType branchType);
	void AddLeafToLastBranch(pugi::xml_node const& objectiveNode);

	void ProgressQuest(
		QuestType t,
		std::vector<std::pair<std::string_view, int>> const& names,
		std::vector<std::pair<int, int>> const& IDs
	) const;

	bool IsQuestCompleted() const;

	void DebugQuests() const;

	std::unordered_set<QuestType> GetQuestTypeSet() const;

private:
	void ParseGeneralProperties(pugi::xml_node const& generalNode);
	void ParseRewardsProperties(pugi::xml_node const& rewardListNode);

	std::string name = "";
	std::string description = "";

	// Key = Type of rewards (Item, Gold, Armor, Weapon, Exp, etc.) 
	// Value = vector of pair<IDs (if needed), amount>
	std::unordered_map<std::string, std::vector<std::pair<int, int>>, StringHash, std::equal_to<>> rewards;

	std::unordered_set<QuestType> objectivesType;

	std::unique_ptr<Quest_Root> root;
};

#endif // __QUEST_H__
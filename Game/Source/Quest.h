#ifndef __QUEST_H__
#define __QUEST_H__

#include "App.h"

#include "Log.h"

#include <string>

enum class QuestType
{
	UNKNOWN			= 0x0000,
	TALK_TO			= 0x0001,
	COLLECT			= 0x0002,
	VISIT			= 0x0004,
	CURRENT_QUEST	= 0x0008,
	NEXT_QUEST		= 0x000F,
	LAST			= 0x0020
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

	virtual void Add(std::unique_ptr<Quest_Component>& component);
	virtual void Add(std::unique_ptr<Quest_Branch>& component);
	virtual void AddLeaf(std::unique_ptr<Quest_Leaf>& component);
	virtual void Remove(Quest_Component* component);

	virtual void Debug() const;

private:
	Quest_Component* parent = nullptr;

};

class Quest_Leaf : public Quest_Component
{
public:

	void ParseXMLGeneralProperties(pugi::xml_node const& objectiveNode);
	bool IsCompleted() const;

	bool CheckCompletion();

	std::pair<int, int> GetProgress() const;

	std::string_view GetObjective() const;

	void ProgressMade(int amount = 1);

	void Debug() const override;

private:
	std::string objective = "";
	bool completed = false;
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

	void SetType(QuestType t);

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

	void Debug() const override;
};

class Quest
{
public:
	Quest() = default;
	explicit Quest(pugi::xml_node const& generalNode);
	~Quest() = default;

	void AddBranch(QuestType branchType);
	void AddLeafToLastBranch(pugi::xml_node const& objectiveNode);

	void DebugQuests() const;

private:
	void ParseGeneralProperties(pugi::xml_node const& node);

	std::string name = "";
	std::string description = "";
	std::unique_ptr<Quest_Root> root;
};

#endif // __QUEST_H__
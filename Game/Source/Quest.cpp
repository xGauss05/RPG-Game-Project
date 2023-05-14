#include "Quest.h"

/*======================================*/
/*		   QuestComponent				*/
/*======================================*/

void Quest_Component::SetParent(Quest_Component* p)
{
	parent = p;
}

Quest_Component* Quest_Component::GetParent() const
{
	return parent;
}

void Quest_Component::Add(std::unique_ptr<Quest_Branch>& component)
{ /* To override if Component wants to use it */}

void Quest_Component::Add(std::unique_ptr<Quest_Component>& component)
{ /* To override if Component wants to use it */}

void Quest_Component::AddLeaf(std::unique_ptr<Quest_Leaf>& component)
{ /* To override if Component wants to use it */}

void Quest_Component::Remove(Quest_Component* component)
{ /* To override if Component wants to use it */}

void Quest_Component::Debug() const
{ /* To override if Component wants to use it */ }



/*======================================*/
/*			 QuestLeaf					*/
/*======================================*/

void Quest_Leaf::ParseXMLGeneralProperties(pugi::xml_node const& objectiveNode)
{
	objective = objectiveNode.attribute("objectiveName").as_string();
	amountNeeded = objectiveNode.attribute("amountNeeded").as_int();
}

bool Quest_Leaf::IsCompleted() const
{
	return completed;
}

bool Quest_Leaf::CheckCompletion()
{
	if (!IsCompleted() && amountAcquired >= amountNeeded)
	{
		GetParent()->Remove(this);
		return completed = true;
	}

	return false;
}

std::pair<int, int> Quest_Leaf::GetProgress() const
{
	return std::make_pair(amountNeeded, amountAcquired);
}

std::string_view Quest_Leaf::GetObjective() const
{
	return objective;
}

void Quest_Leaf::ProgressMade(int amount)
{
	amountAcquired += amount;
	CheckCompletion();
}

void Quest_Leaf::Debug() const
{
	std::string debugSTR = std::format(
		"Objective: {} | Completed: {} | AmountNeeded: {} | AmountAcquired: {}",
		objective, completed, amountNeeded, amountAcquired
	);
	LOG(debugSTR.c_str());
}



/*======================================*/
/*			 QuestBranch				*/
/*======================================*/

void Quest_Branch::Add(std::unique_ptr<Quest_Component>& component)
{
	component->SetParent(this);
	children.push_back(std::move(component));
}
void Quest_Branch::Add(std::unique_ptr<Quest_Branch>& component)
{
	component->SetParent(this);
	children.push_back(std::move(component));
}
void Quest_Branch::AddLeaf(std::unique_ptr<Quest_Leaf>& component)
{
	component->SetParent(this);
	children.push_back(std::move(component));
}

void Quest_Branch::Remove(Quest_Component* component)
{
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		if (it->get() == component)
		{
			children.erase(it);
			return;
		}
	}

	component->SetParent(nullptr);
}

bool Quest_Branch::HasChildren() const
{
	return !children.empty();
}

void Quest_Branch::SetType(QuestType t)
{
	type = t;
}

void Quest_Branch::Debug() const
{
	for (auto const& elem : children)
	{
		elem->Debug();
	}
}



/*======================================*/
/*			 QuestRoot					*/
/*======================================*/

void Quest_Root::AddNextQuest(std::unique_ptr<Quest_Root>& component)
{
	component->SetParent(this);
	children.push_back(std::move(component));
}

void Quest_Root::AddLeafToLastBranchAdded(std::unique_ptr<Quest_Leaf>& component)
{
	children.back()->AddLeaf(component);
}

void Quest_Root::Debug() const
	{
		for (auto const& elem : children)
		{
			elem->Debug();
		}
	}



/*======================================*/
/*			    Quest					*/
/*======================================*/

Quest::Quest(pugi::xml_node const& generalNode)
{
	if (generalNode)
	{
		ParseGeneralProperties(generalNode);
	}

	root = std::make_unique<Quest_Root>();
	root->SetType(QuestType::CURRENT_QUEST);
}

void Quest::AddBranch(QuestType branchType)
{
	if (branchType == QuestType::NEXT_QUEST)
	{
		auto branchPtr = std::make_unique<Quest_Root>();
		branchPtr->SetType(branchType);
		root->AddNextQuest(branchPtr);
	}
	else
	{
		auto branchPtr = std::make_unique<Quest_Branch>();
		branchPtr->SetType(branchType);
		root->Add(branchPtr);
	}
}

void Quest::AddLeafToLastBranch(pugi::xml_node const& objectiveNode)
{
	auto leafPtr = std::make_unique<Quest_Leaf>();
	leafPtr->ParseXMLGeneralProperties(objectiveNode);
	root->AddLeafToLastBranchAdded(leafPtr);
}

void Quest::DebugQuests() const
{
	root->Debug();
}

void Quest::ParseGeneralProperties(pugi::xml_node const& generalNode)
{
	name = generalNode.attribute("name").as_string();
	description = generalNode.child("description").text().as_string();
}
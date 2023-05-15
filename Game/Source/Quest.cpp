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

void Quest_Component::ToggleEnabled()
{
	enabled = !enabled;
}

bool Quest_Component::IsEnabled() const
{
	return enabled;
}

QuestType Quest_Component::GetType() const
{
	return QuestType::NONE;
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

void Quest_Component::ProgressQuest(std::vector<std::pair<std::string_view, int>> const& names, std::vector<std::pair<int, int>> const& IDs)
{ /* To override if Component wants to use it */ }

void Quest_Component::CheckChildrenProgress()
{ /* To override if Component wants to use it */ }

void Quest_Component::FillObjectiveInfo(std::vector<Quest_Display>& display) const
{ /* To override if Component wants to use it */ }


/*======================================*/
/*			 QuestLeaf					*/
/*======================================*/

void Quest_Leaf::ParseXMLGeneralProperties(pugi::xml_node const& objectiveNode)
{
	objectiveName = objectiveNode.attribute("objectiveName").as_string();
	objectiveID = objectiveNode.attribute("objectiveID").as_int();
	amountNeeded = objectiveNode.attribute("amountNeeded").as_int();
}

bool Quest_Leaf::CheckCompletion()
{
	if (IsEnabled() && amountAcquired >= amountNeeded)
	{
		ToggleEnabled();
		GetParent()->CheckChildrenProgress();

		return true;
		//GetParent()->Remove(this);
	}

	return false;
}

std::pair<int, int> Quest_Leaf::GetCurrentProgress() const
{
	return std::make_pair(amountNeeded, amountAcquired);
}

void Quest_Leaf::ProgressQuest(std::vector<std::pair<std::string_view, int>> const& names, std::vector<std::pair<int, int>> const& IDs)
{
	for (auto const& [name, amount] : names)
	{
		if (StrEquals(name, objectiveName))
		{
			AddToProgress(amount);
		}
	}

	for (auto const& [id, amount] : IDs)
	{
		if (id == objectiveID)
		{
			AddToProgress(amount);
		}
	}
}

void Quest_Leaf::AddToProgress(int amount)
{
	amountAcquired += amount;
	CheckCompletion();
}

std::pair<std::string_view, int> Quest_Leaf::GetObjective() const
{
	return std::pair(objectiveName, objectiveID);
}

void Quest_Leaf::FillObjectiveInfo(std::vector<Quest_Display>& display) const
{
	display.back().objectiveName = objectiveName;
	display.back().objectiveID = objectiveID;
	display.back().amountToDisplay = std::format("{}/{}", amountAcquired, amountNeeded);
}

void Quest_Leaf::Debug() const
{
	std::string debugSTR = std::format(
		"ObjectiveName: {} | ObjectiveID: {} | AmountNeeded: {} | AmountAcquired: {}",
		objectiveName, objectiveID, amountNeeded, amountAcquired
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

void Quest_Branch::ProgressQuest(std::vector<std::pair<std::string_view, int>> const& names, std::vector<std::pair<int, int>> const& IDs)
{
	for (auto const& elem : children)
	{
		if (elem->IsEnabled())
		{
			elem->ProgressQuest(names, IDs);
		}
	}
}

void Quest_Branch::CheckChildrenProgress()
{
	for (auto const& elem : children)
	{
		if (elem->IsEnabled())
		{
			return;
		}
	}

	ToggleEnabled();

	GetParent()->CheckChildrenProgress();
}

void Quest_Branch::FillObjectiveInfo(std::vector<Quest_Display>& display) const
{
	for (auto const& elem : children)
	{
		display.emplace_back();
		switch (GetType())
		{
			using enum QuestType;
		case TALK_TO:
			display.back().base = "Talk to {}.";
			break;
		case COLLECT:
			display.back().base = "Collect {}.";
			break;
		case VISIT:
			display.back().base = "Visit {}.";
			break;
		case HUNT:
			display.back().base = "Kill {}.";
			break;
		default:
			continue;
		}
		elem->FillObjectiveInfo(display);
	}
}

void Quest_Branch::SetType(QuestType t)
{
	type = t;
}

QuestType Quest_Branch::GetType() const
{
	return type;
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

void Quest_Root::CheckIfObjectiveType(QuestType t, std::vector<std::pair<std::string_view, int>> const& names, std::vector<std::pair<int, int>> const& IDs) const
{
	for (auto const& elem : children)
	{
		if (elem->IsEnabled() && elem->GetType() == t)
		{
			elem->ProgressQuest(names, IDs);
		}
	}
}

void Quest_Root::CheckChildrenProgress()
{
	for (auto const& elem : children)
	{
		if (elem->IsEnabled())
		{
			return;
		}
	}

	if(!IsQuestCompleted())
	{
		ToggleQuestCompleted();
		ToggleEnabled();
	}
}

void Quest_Root::FillObjectiveInfo(std::vector<Quest_Display>& display) const
{
	for (auto const& elem : children)
	{
		using enum QuestType;

		QuestType displayTypes = UNKNOWN;
		displayTypes = TALK_TO | COLLECT | VISIT | HUNT;

		if ((displayTypes & elem->GetType()) == elem->GetType())
		{
			elem->FillObjectiveInfo(display);
		}
	}
}

void Quest_Root::ToggleQuestCompleted()
{
	questCompleted = !questCompleted;
}

bool Quest_Root::IsQuestCompleted() const
{
	return questCompleted;
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

Quest::Quest(pugi::xml_node const& questNode)
{
	if (auto generalNode = questNode.child("general");
		generalNode)
	{
		ParseGeneralProperties(generalNode);
	}

	if (auto rewardListNode = questNode.child("rewardlist");
		rewardListNode)
	{
		ParseRewardsProperties(rewardListNode);
	}

	root = std::make_unique<Quest_Root>();
	root->SetType(QuestType::CURRENT_QUEST);
}

void Quest::AddBranch(QuestType branchType)
{
	objectivesType.insert(branchType);

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

void Quest::ProgressQuest(QuestType t, std::vector<std::pair<std::string_view, int>> const& names, std::vector<std::pair<int, int>> const& IDs) const
{
	root->CheckIfObjectiveType(t, names, IDs);
}

bool Quest::IsQuestCompleted() const
{
	return root->IsQuestCompleted();
}

void Quest::DebugQuests() const
{
	root->Debug();
}

std::unordered_set<QuestType> Quest::GetQuestTypeSet() const
{
	return objectivesType;
}

std::string_view Quest::GetQuestName() const
{
	return name;
}

std::string_view Quest::GetQuestDescription() const
{
	return description;
}

std::vector<Quest_Display> Quest::GetQuestDisplayInfo() const
{
	std::vector<Quest_Display> returnDisplay;
	root->FillObjectiveInfo(returnDisplay);
	return returnDisplay;
}
void Quest::ParseGeneralProperties(pugi::xml_node const& generalNode)
{
	name = generalNode.attribute("name").as_string();
	description = generalNode.child("description").text().as_string();
}

void Quest::ParseRewardsProperties(pugi::xml_node const& rewardListNode)
{
	for (auto const& rewardNode : rewardListNode)
	{
		rewards[rewardNode.name()].emplace_back(
			rewardNode.attribute("id").as_int(),
			rewardNode.attribute("amount").as_int()
		);
	}
}

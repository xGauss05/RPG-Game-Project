#ifndef __QUESTMANAGER_H__
#define __QUESTMANAGER_H__

#include "Module.h"
#include "Quest.h"

#include "Defs.h"		//StringHash

#include <vector>
#include <memory>

class Quest;

class QuestManager : public Module
{
public:

	QuestManager();

	// Destructor
	~QuestManager() final;

	// Called before render is available
	bool Awake(pugi::xml_node& config) final;

	// Called before the first frame
	bool Start() final;

	// Called before all Updates
	bool PreUpdate() final;

	bool Pause(int phase) final;

	// Called each loop iteration
	bool Update(float dt) final;

	// Called before all Updates
	bool PostUpdate() final;

	// Called before quitting
	bool CleanUp() final;

	bool HasSaveData() const final;
	bool LoadState(pugi::xml_node const &data) final;
	pugi::xml_node SaveState(pugi::xml_node const &) const final;

	void ActivateQuest(std::string name);
	void DeactivateQuest(std::string name);

private:

	std::vector<std::unique_ptr<Quest>> quests;
	std::vector<std::unique_ptr<Quest>> activeQuests;
	std::vector<std::unique_ptr<Quest>> completedQuests;
};

#endif // __QUESTMANAGER_H__
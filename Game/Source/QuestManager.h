#ifndef __QUESTMANAGER_H__
#define __QUESTMANAGER_H__

#include "Module.h"
#include "Quest.h"

#include "Defs.h"		//StringHash

#include <vector>

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

private:

	std::unique_ptr<Quest> quests;
	std::unique_ptr<Quest> activeQuests;
	std::unique_ptr<Quest> completedQuests;

	//LookUpXMLNodeFromString questsInfo;
};

#endif // __QUESTMANAGER_H__
#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "Defs.h"

#include <memory>
#include <vector>

struct TileInfo;

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	~EntityManager() final;

	// Called before render is available
	bool Awake(pugi::xml_node&) final;

	// Called after Awake
	bool Start() final;

	// Called every frame
	bool PreUpdate() final;
	void RestartLevel() const;
	bool Update(float dt) final;
	bool PostUpdate() final;

	// Called when game is paused
	bool Pause(int phase) final;

	// Called before quitting
	bool CleanUp() final;

	// ------ Entity
	// --- Constructors
	void CreateEntity(std::string const &type, pugi::xml_node const &parameters = pugi::xml_node());
	// --- Destructors
	bool DestroyEntity(std::string const &type,  int id);

	// ------ Load Assets
	bool LoadAllTextures() const;
	bool LoadEntities(TileInfo const *tileInfo, iPoint pos, int width, int height);

private:
	// ------ Utils
	// --- Getters
	pugi::xml_node SaveState(pugi::xml_node const &data) const final;
	bool HasSaveData() const final;
	bool DoesEntityExist(Entity const *entity = nullptr) const;
	bool IsEntityActive(Entity const *entity = nullptr) const;

	std::unordered_map<std::string, std::vector<std::unique_ptr<Entity>>, StringHash, std::equal_to<>> allEntities;
};


#endif // __ENTITYMANAGER_H__

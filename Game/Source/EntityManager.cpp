#include "EntityManager.h"
#include "App.h"

#include "Map.h"
#include "Window.h"

#include "Defs.h"
#include "Log.h"

#include <regex>
#include <locale>		// std::tolower

EntityManager::EntityManager() : Module()
{
	name = "entitymanager";
}

// Destructor
EntityManager::~EntityManager() = default;

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");

	for(auto const &[entityType, entityInfo] : allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(!IsEntityActive(entity.get())) continue;
			if(!entity->Awake()) return false;
		}
	}
	return true;
}

bool EntityManager::Start() 
{
	for(auto const &[entityType, entityInfo] : allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(!IsEntityActive(entity.get())) continue;
			if(!entity->Start()) return false;
		}
	}

	return true;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	for(auto const &[entityType, entityInfo] : allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(!DoesEntityExist(entity.get())) continue;
			if(!entity->CleanUp()) return false;
		}
	}
	return true;
}

void EntityManager::CreateEntity(std::string const& type, pugi::xml_node const& parameters)
{
}

bool EntityManager::DestroyEntity(std::string const &type, int id)
{
	if(id < 0 || !allEntities.contains(type)) return false;

	allEntities[type].at(id)->Stop();
	allEntities[type].at(id).reset();
	allEntities[type].erase(allEntities[type].begin() + id);

	return true;
}

bool EntityManager::LoadAllTextures() const
{
	return true;
}

bool EntityManager::LoadEntities(TileInfo const *tileInfo, iPoint pos, int width, int height)
{
	return true;
}

bool EntityManager::IsEntityActive(Entity const *entity) const
{
	if(!entity) return false;
	if(!entity->active) return false;
	return true;
}

bool EntityManager::DoesEntityExist(Entity const *entity) const
{
	if(entity) return true;
	return false;
}

bool EntityManager::PreUpdate()
{
	for(auto const &[entityType, entityInfo] : allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(entity->disableOnNextUpdate) entity->Stop();
		}
	}
	return true;
};

void EntityManager::RestartLevel() const
{
	for(auto const &[entityType, entityInfo] : allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(!entity->active) entity->Start();
			entity->RestartLevel();
		}
	}
}

bool EntityManager::PostUpdate()
{
	for(auto const &[entityType, entityInfo] : allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(!IsEntityActive(entity.get())) continue;
		}
	}
	return true;
}

bool EntityManager::Update(float dt)
{
	for(auto const &[entityType, entityInfo] : allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(!IsEntityActive(entity.get())) continue;
			if(!entity->Update()) return false;
		}
	}
	return true;
}

bool EntityManager::Pause(int phase)
{
	for(auto const &[entityType, entityInfo] : allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(!IsEntityActive(entity.get())) continue;
			if(!entity->Pause()) return false;
		}
	}
	return true;
}

pugi::xml_node EntityManager::SaveState(pugi::xml_node const &data) const
{
	pugi::xml_node temp = data;
	temp = temp.append_child("entitymanager");
	for(auto const &[name, entityInfo]: allEntities)
	{
		for(auto const &entity : entityInfo)
		{
			if(!entity->HasSaveData()) continue;
			entity->SaveState(temp);
		}
	}
	return temp;
}

bool EntityManager::HasSaveData() const
{
	return true;
}

#include "Entity.h"
#include "App.h"

// Modules included
#include "Input.h"
#include "Render.h"

// Utils included
#include "Log.h"

// Libraries included
#include "dirent.h"

#include <regex>

Entity::Entity(pugi::xml_node const &itemNode, int newId) : id(newId), parameters(itemNode) {}

bool Entity::Awake()
{
	return true;
}

bool Entity::Start()
{
	SpawnEntity();
	active = true;
	return true;
}

bool Entity::Update()
{
	return true;
}

bool Entity::Pause() const
{
	return true;
}

bool Entity::CleanUp()
{
	return true;
}

bool Entity::Stop()
{
	disableOnNextUpdate = false;
	return true;
}

void Entity::SpawnEntity()
{
	disableOnNextUpdate = false;
	position = startingPosition;
}

bool Entity::HasSaveData() const
{
	return false;
}

bool Entity::LoadState(pugi::xml_node const &)
{
	return true;
}

pugi::xml_node Entity::SaveState(pugi::xml_node const &)
{
	return pugi::xml_node();
}

void Entity::Enable()
{
	if(!active)
	{
		active = true;
		Start();
	}
}

void Entity::Disable()
{
	if(active)
	{
		active = false;
		disableOnNextUpdate = true;
	}
}

void Entity::RestartLevel()
{
	position = startingPosition;
}
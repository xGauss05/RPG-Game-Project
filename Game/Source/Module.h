#ifndef __MODULE_H__
#define __MODULE_H__

#include <string>

#include "PugiXml/src/pugixml.hpp"

class App;
class PhysBody;

class Module
{
public:

	Module() = default;
	
	virtual ~Module() = default;

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called when game is paused
	virtual bool Pause(int phase)
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}
	
	virtual bool LoadState(pugi::xml_node const &)
	{
		return true;
	}

	virtual pugi::xml_node SaveState(pugi::xml_node const &)  const
	{
		return pugi::xml_node();
	}

	virtual void OnCollisionStart(PhysBody* bodyA, PhysBody* bodyB)
	{
		// To Override
	}

	virtual bool HasSaveData() const
	{
		return false;
	}

	std::string name;
	bool active = false;
};

#endif // __MODULE_H__
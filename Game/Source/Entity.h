#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "PugiXml/src/pugixml.hpp"

enum class RenderModes
{
	IMAGE = 0,
	ANIMATION,
	NO_RENDER,
	UNKNOWN
};

class Entity
{
public:

	explicit Entity() = default;
	explicit Entity(pugi::xml_node const &itemNode, int newId);
	virtual ~Entity() = default;

	virtual bool Awake();
	void Enable();
	virtual bool Start();
	virtual void SpawnEntity();
	virtual void RestartLevel();

	void Disable();
	virtual bool Stop();

	virtual bool Update();
	virtual bool Pause() const;
	
	virtual bool CleanUp();

	virtual bool HasSaveData() const;
	virtual bool LoadState(pugi::xml_node const &data);
	virtual pugi::xml_node SaveState(pugi::xml_node const &data);

	virtual void BeforeCollisionStart() { /* Method to Override */ };
	virtual void OnCollisionStart() { /* Method to Override */ };
	virtual void OnCollisionEnd() { /* Method to Override */ };

	virtual void DrawDebug() const { /* Method to Override */ };

	bool active = true;

	bool disableOnNextUpdate = false;

	int id = -1;
	std::string name = "unknown";

	iPoint position;
	iPoint startingPosition;
	iPoint colliderOffset;

	//std::shared_ptr<Animation> texture;

	pugi::xml_node parameters;
	std::string texturePath;
	std::string fxPath;
};

#endif // __ENTITY_H__
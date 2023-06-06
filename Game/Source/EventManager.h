#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Event_Base.h"

#include <memory>
#include <vector>
#include <unordered_set>
#include <chrono>

struct TileInfo;

struct AmbienceSFX
{
	int baseCooldown = 10;
	int cooldownVariance = 0;
	int maxCooldown = INT_MAX;
	int minCooldown = 0;
	int randomChance = 100;
	std::string path = "";
	int sfxID = 0;

	std::chrono::time_point<std::chrono::steady_clock> lastTimePlayed;
	int cooldown = 10;

	explicit AmbienceSFX(pugi::xml_node const& node);

	void ReadProperty(pugi::xml_node const& node);
	
	void CalculateNewCooldown();
};

class EventManager
{
public:

	EventManager();

	// Destructor
	~EventManager();

	void Initialize();

	// ------ Event
	// --- Constructors
	bool CreateEvents(
		Publisher &publisher,
		pugi::xml_node const &node = pugi::xml_node()
	);

	void SubscribeEventsToGlobalSwitches() const;

	bool IsWalkable(iPoint position) const;

	void DrawEvents() const;

	EventTrigger TriggerActionButtonEvent(iPoint position) const;
	EventTrigger TriggerPlayerTouchEvent(iPoint position) const;

	std::vector<AmbienceSFX>& GetPeriodicSFXs();

	void SetEventsTilesetPath(std::string_view path, int gid);

private:
	const int tileSize = 48;
	void LoadEventTextures() const;
	std::pair<EventTrigger, bool> TriggerEvent(iPoint position, Event_Base * const event) const;

	std::vector<std::unique_ptr<Event_Base>> events;

	std::vector<AmbienceSFX> periodicSFXs;

	std::string eventsTileset = "";
	int firstGID = -1;
};


#endif // __ENTITYMANAGER_H__

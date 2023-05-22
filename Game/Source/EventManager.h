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

	int GetEventLayerSize() const;

	bool IsWalkable(iPoint position) const;

	void RedrawnCompleted();

	EventTrigger TriggerActionButtonEvent(iPoint position) const;
	EventTrigger TriggerPlayerTouchEvent(iPoint position) const;

	std::vector<AmbienceSFX>& GetPeriodicSFXs();

	// Returns Gid, position, keepDrawing ? true : false;
	std::tuple<int, iPoint, bool> GetDrawEventInfo(int index = 0);
	std::pair<int, iPoint> GetRedrawEventGID(iPoint position);

	void SetEventsTilesetPath(std::string_view path);

private:
	const int tileSize = 48;
	std::pair<EventTrigger, bool> TriggerEvent(iPoint position, Event_Base * const event) const;

	std::vector<std::unique_ptr<Event_Base>> events;

	std::vector<std::unique_ptr<Event_Base>>::const_iterator drawIterator;

	std::unordered_set<Event_Base*> alreadyRedrawnEvents;

	std::vector<AmbienceSFX> periodicSFXs;

	std::string eventsTileset = "";
};


#endif // __ENTITYMANAGER_H__

#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Event_Base.h"

#include <memory>
#include <vector>

struct TileInfo;

class EventManager
{
public:

	EventManager();

	// Destructor
	~EventManager();

	void Initialize();

	// ------ Event
	// --- Constructors
	bool CreateEvent(
		Publisher &publisher,
		pugi::xml_node const &node = pugi::xml_node()
	);

	void SubscribeEventsToGlobalSwitches();

	int GetEventLayerSize() const;

	bool IsWalkable(iPoint position) const;

	EventTrigger TriggerActionButtonEvent(iPoint position) const;
	EventTrigger TriggerPlayerTouchEvent(iPoint position) const;

	

	// Returns Gid, position, keepDrawing ? true : false;
	std::tuple<int, iPoint, bool> GetDrawEventInfo(int index = 0);

private:
	const int tileSize = 48;
	std::pair<EventTrigger, bool> TriggerEvent(iPoint position, Event_Base * const event) const;

	std::vector<std::unique_ptr<Event_Base>> events;

	std::vector<std::unique_ptr<Event_Base>>::const_iterator drawIterator;
};


#endif // __ENTITYMANAGER_H__

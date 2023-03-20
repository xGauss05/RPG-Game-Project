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
	bool CreateEvent(pugi::xml_node const &node = pugi::xml_node());

	int GetEventLayerSize() const;

	// Returns Gid, position, keepDrawing ? true : false;
	std::tuple<int, iPoint, bool> GetDrawEventInfo(int index = 0);
private:
	std::vector<std::unique_ptr<Event_Base>> events;

	std::vector<std::unique_ptr<Event_Base>>::const_iterator drawIterator;
};


#endif // __ENTITYMANAGER_H__

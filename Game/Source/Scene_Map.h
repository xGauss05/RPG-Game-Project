#ifndef __SCENE_MAP_H__
#define __SCENE_MAP_H__

#include "Scene_Base.h"
#include "Player.h"
#include "Map.h"

class Scene_Map : public Scene_Base
{
public:
	Scene_Map() = default;
	explicit Scene_Map(std::string const& newMap);
	explicit Scene_Map(std::string const& newMap, iPoint playerCoords);

	bool isReady() override;
	void Load(
		std::string const& path,
		LookUpXMLNodeFromString const& info,
		Window_Factory const& windowFac
	) override;
	void Start() override;
	void Draw() override;
	TransitionScene Update() override;
	int OnPause() override;
	int CheckNextScene() override;
	bool SaveScene(pugi::xml_node const&) override;
	bool LoadScene(pugi::xml_node const&) override;
	void DebugDraw() override;

	std::string_view GetNextMap() const;
	iPoint GetTPCoordinates() const;

private:

	enum class MapState
	{
		NORMAL,
		ON_MESSAGE,
		ON_DIALOG,
		ON_MENU_SELECTION
	};

	bool godMode = false;

	std::string currentMap = "";

	EventTrigger tpInfo;

	Map map;
	Player player;

	MapState state = MapState::NORMAL;

	pugi::xml_document currentDialogDocument;
	pugi::xml_node currentDialogNode;

	TransitionScene transitionTo = TransitionScene::NONE;

	const Window_Factory* windowFactory = nullptr;
	LookUpXMLNodeFromString xmlNode; //Maybe remove that when fixed?

	std::vector<std::unique_ptr<Window_Base>> pauseWindow;
};


#endif __SCENE_MAP_H__
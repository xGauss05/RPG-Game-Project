#ifndef __SCENE_MAP_H__
#define __SCENE_MAP_H__

#include "Scene_Base.h"
#include "Player.h"
#include "Map.h"

class Scene_Map : public Scene_Base
{
public:
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


private:

	enum class MapState
	{
		NORMAL,
		ON_MESSAGE,
		ON_DIALOG
	};

	std::string currentMap = "";

	Map map;
	Player player;

	MapState state = MapState::NORMAL;

	pugi::xml_document currentDialogDocument;
	pugi::xml_node currentDialogNode;

	bool exit = false;

	TransitionScene transitionTo = TransitionScene::NONE;

	const Window_Factory* windowFactory;
	LookUpXMLNodeFromString xmlNode; //Maybe remove that when fixed?

	std::vector<std::unique_ptr<Window_Base>> pauseWindow;
};


#endif __SCENE_MAP_H__
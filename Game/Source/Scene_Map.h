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
        Window_Factory const& windowFactory
    ) override;
    void Start() override;
    void Draw() override;
    TransitionScene Update() override;
    int OnPause() override;
    int CheckNextScene() override;

    std::vector<std::unique_ptr<Window_Base>> pauseWindow;

private:
    std::string currentMap = "";

    Map map;
    Player player;

   
    bool exit = false;

    const Window_Factory* windowFactory;
    LookUpXMLNodeFromString xmlNode; //Maybe remove that when fixed?

};


#endif __SCENE_MAP_H__
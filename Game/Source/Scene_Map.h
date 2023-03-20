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
    int Update() override;
    int CheckNextScene() override;

private:
    std::string currentMap = "";

    Map map;
    Player player;
};


#endif __SCENE_MAP_H__
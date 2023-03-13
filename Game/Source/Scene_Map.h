#pragma once
#include "Scene_Base.h"
#include "Map.h"
class Scene_Map :
    public Scene_Base
{
public:

    // Inherited via Scene_Base
    bool isReady() override;
    void Load(std::string const& path) override;
    void Start() override;
    void Update() override;

    std::string currentMap = "";

    Map map;
};


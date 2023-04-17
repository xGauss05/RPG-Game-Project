#ifndef __SCENE_BOOT_H__
#define __SCENE_BOOT_H__

#include "Scene_Base.h"
#include "TextureManager.h"
#include "Log.h"

#include <chrono>
#include <stdlib.h>

class Scene_Boot : public Scene_Base
{
public:
    ~Scene_Boot();

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
    bool SaveScene(pugi::xml_node const&) override;
    bool LoadScene(pugi::xml_node const&) override;
    void DebugDraw() override;

private:
    int backgroundTexture;
    int logoFx = 0;
    bool playedLogo = false;
    bool transition = false;

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point currentTime;
    std::chrono::milliseconds elapsed;
};

#endif __SCENE_BOOT_H__
#ifndef __SCENE_TITLE_H__
#define __SCENE_TITLE_H__

#include "Scene_Base.h"
#include "TextureManager.h"
#include "Log.h"
#include "Easing.h"

class Scene_Title : public Scene_Base
{
public:
    ~Scene_Title();

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
    Easing easing;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point current;
    bool started = true;
};

#endif __SCENE_TITLE_H__
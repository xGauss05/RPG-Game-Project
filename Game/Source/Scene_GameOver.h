#ifndef __SCENE_GAMEOVER_H__
#define __SCENE_GAMEOVER_H__

#include "Scene_Base.h"
#include "TextureManager.h"
#include "Log.h"

class Scene_GameOver : public Scene_Base
{
public:
    ~Scene_GameOver();

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

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point current;

};

#endif __SCENE_GAMEOVER_H__
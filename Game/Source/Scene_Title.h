#ifndef __SCENE_TITLE_H__
#define __SCENE_TITLE_H__

#include "Scene_Base.h"
#include "TextureManager.h"
#include "Log.h"

class Scene_Title : public Scene_Base
{
public:
    ~Scene_Title() override;

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

    // TODO: Can this be private?
    void DoImagesEasing();
    void DoButtonsEasing();

private:
    int backgroundTexture;
    int titleTexture;
    int studioTexture;
    int logoFx = 0;
    bool playedLogo = false;

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point current;

    // Why is it a vector if there's one, and only one, optionsWindow
    std::vector<std::unique_ptr<Window_Base>> optionsWindow;
};

#endif __SCENE_TITLE_H__
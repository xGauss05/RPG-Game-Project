#ifndef __SCENE_TITLE_H__
#define __SCENE_TITLE_H__

#include "Scene_Base.h"
#include "TextureManager.h"
#include "Log.h"

class Scene_Title : public Scene_Base
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
    int backgroundTexture;
};

#endif __SCENE_TITLE_H__
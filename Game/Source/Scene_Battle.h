#pragma once
#include "Scene_Base.h"
#include "GameParty.h"
#include "EnemyTroops.h"

enum class BattleState
{
    PLAYER_INPUT,
    ENEMY_INPUT,
    RESOLUTION
};

class Scene_Battle : public Scene_Base
{
public:
    explicit Scene_Battle(GameParty *gameParty, EnemyTroops enemyTroops);
    bool isReady() override;
    void Load(
        std::string const& path,
        LookUpXMLNodeFromString const& info,
        Window_Factory const& windowFactory
    ) override;
    void Start() override;
    void Draw() override;
    TransitionScene Update() override;
    int CheckNextScene() override;

private:
    GameParty* party;
    EnemyTroops troop;
    BattleState state = BattleState::PLAYER_INPUT;
};


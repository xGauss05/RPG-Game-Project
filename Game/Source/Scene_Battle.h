#pragma once
#include "Scene_Base.h"
#include "GameParty.h"
#include "EnemyTroops.h"

#include <queue>

enum class BattleState
{
    PLAYER_INPUT,
    ENEMY_INPUT,
    RESOLUTION
};

class Scene_Battle : public Scene_Base
{
public:
    explicit Scene_Battle(GameParty *gameParty, std::string const &fightName);
    bool isReady() override;
    void Load(
        std::string const& path,
        LookUpXMLNodeFromString const& info,
        Window_Factory const& windowFactory
    ) override;
    void Start() override;
    void Draw() override;
    TransitionScene Update() override;
    bool CheckBattleWin() const;
    bool CheckBattleLoss() const;
    int CheckNextScene() override;
    int OnPause() override;

private:
    void DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const;
    void ChooseTarget();

    GameParty* party;
    EnemyTroops enemies;
    BattleState state = BattleState::PLAYER_INPUT;
    int currentPlayer = 0;
    bool showNextText = true;
    int targetSelected = -1;
    int actionSelected = -1;

    struct BattleAction
    {
        int action;
        int source;
        int target;
        bool friendly;
        int speed;
    };

    struct CompareActionSpeed
    {
        bool operator()(const BattleAction& a1, const BattleAction& a2) const
        {
            return a1.speed < a2.speed;
        }
    };

    std::priority_queue<BattleAction, std::vector<BattleAction>, CompareActionSpeed> actionQueue;
};


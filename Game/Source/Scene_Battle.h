#pragma once
#include "Scene_Base.h"
#include "GameParty.h"
#include "EnemyTroops.h"

#include <random>

#include <queue>

enum class BattleState
{
    PLAYER_INPUT,
    ENEMY_INPUT,
    RESOLUTION,
    BATTLE_WON,
    BATTLE_LOSS
};

class Scene_Battle : public Scene_Base
{
public:
    explicit Scene_Battle(GameParty *gameParty, std::string_view fightName = "");
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
    bool SaveScene(pugi::xml_node const&) override;
    bool LoadScene(pugi::xml_node const&) override;
    void DebugDraw() override;

private:
    void DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const;
    bool ChooseTarget();
    std::string_view GetRandomEncounter();

    int backgroundTexture = 0;

    GameParty* party;
    EnemyTroops enemies;
    BattleState state = BattleState::PLAYER_INPUT;
    int currentPlayer = 0;
    bool showNextText = true;
    int targetSelected = -1;
    int actionSelected = -1;

    std::uniform_int_distribution<> random;
    std::uniform_int_distribution<> random40;
    std::uniform_int_distribution<> random100;
    
    std::random_device rd;

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

    std::unique_ptr<Window_List> actions;
    std::unique_ptr<Window_Panel> messages;

    std::priority_queue<BattleAction, std::vector<BattleAction>, CompareActionSpeed> actionQueue;

    // Sound indexes
    int attackSfx = -1;
    int criticalSfx = -1;
    int blockSfx = -1;
    int escapeSfx = -1;
    int enemydeadSfx = -1;
    int erYonaTurnSfx = -1;
    int rocioTurnSfx = -1;
    int antonioTurnSfx = -1;
    int sayoriTurnSfx = -1;

    bool playedTurnSfx = false;
};

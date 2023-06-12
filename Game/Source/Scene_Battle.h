#ifndef __SCENE_BATTLE_H__
#define __SCENE_BATTLE_H__

#include "Scene_Base.h"
#include "GameParty.h"
#include "EnemyTroops.h"

#include "ParticleDB.h"

#include "Battle_Window_Menu.h"
#include "GuiSegmentedMessageBox.h"


#include <random>

#include <queue>

class Scene_Battle : public Scene_Base
{
public:
    explicit Scene_Battle(GameParty *gameParty, std::string_view fightName = "");
    ~Scene_Battle() override;
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
    int OnPause() override;
    bool SaveScene(pugi::xml_node const&) override;
    bool LoadScene(pugi::xml_node const&) override;
    void DebugDraw() override;

private:
    enum class BattleState
    {
        PLAYER_INPUT,
        ENEMY_INPUT,
        RESOLUTION,
        BATTLE_WON,
        BATTLE_LOSS,
        RUN_FROM_BATTLE
    };

    std::string_view GetRandomEncounter();

    void UpdatePlayerTurn();
    void UpdatePlayerInputText();

    bool IsAdvanceTextButtonDown() const;

    void ChooseEnemyActions();

    void ResolveActionQueue();
    std::string ResolveAction(BattleAction const& currentAction);
    std::string BattlerDefending(Battler &battler) const;
    std::string BattlerAttacking(Battler const &source, Battler& receiver, BaseStats offensiveStat, BaseStats defensiveStat);
    int CalculateDamage(int atk, int def, bool defending = false, float crit = 1.0f);
    void BattlerJustDied(Battler const& battler);

    void CheckIfBattleWinThenChangeState();
    void ResolveWinningBattle();

    void CheckBattleLossThenChangeState();

    void PlayBattlerSFX(Battler const &battler) const;
    void PlayActionSFX(std::string_view sfxKey) const;

    void DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const;

    int backgroundTexture = 0;

    GameParty* party = nullptr;
    std::unique_ptr<EnemyTroops> enemies;

    bool checkBattleEnd = false;

    BattleState state = BattleState::PLAYER_INPUT;
    int currentPlayer = 0;
    int targetSelected = -1;
    ActionNames actionSelected = ActionNames::NONE;

    std::uniform_int_distribution<> random;
    std::uniform_int_distribution<> random40;
    std::uniform_int_distribution<> random100;
    
    std::random_device rd;

    struct CompareActionSpeed
    {
        bool operator()(const BattleAction& a1, const BattleAction& a2) const
        {
            return a1.speed < a2.speed;
        }
    };

    std::unique_ptr<Battle_Window_Menu> menu;
    GuiSegmentedMessageBox messages;

    std::queue<std::string> messageQueue;

    std::priority_queue<BattleAction, std::vector<BattleAction>, CompareActionSpeed> actionQueue;

    std::unordered_map<std::string_view, int> sfx;

    bool bBattleResolved = false;
    int DebugAmount = 0;

    ParticleDB particleDB;
 };

#endif //__SCENE_BATTLE_H__

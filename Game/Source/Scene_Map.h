#ifndef __SCENE_MAP_H__
#define __SCENE_MAP_H__

#include "Scene_Base.h"
#include "GameParty.h"
#include "Player.h"
#include "Map.h"
#include <unordered_set>

#include <random>

class Scene_Map : public Scene_Base
{
public:
	Scene_Map() = default;
	explicit Scene_Map(GameParty* party);
	explicit Scene_Map(std::string const& newMap, GameParty* party);
	explicit Scene_Map(std::string const& newMap, iPoint playerCoords, GameParty* party);

	bool isReady() override;
	void Load(
		std::string const& path,
		LookUpXMLNodeFromString const& info,
		Window_Factory const& windowFac
	) override;
	void Start() override;
	void Draw() override;
	TransitionScene Update() override;
	int OnPause() override;
	int CheckNextScene() override;
	bool SaveScene(pugi::xml_node const&) override;
	bool LoadScene(pugi::xml_node const&) override;
	void DebugDraw() override;
	void UpdateStatsMenu();
	std::string_view GetNextMap() const;
	iPoint GetTPCoordinates() const;
	TransitionScene TryRandomBattle();

	void SetPlayerParty(GameParty* party);

private:
	void DrawStatsMenu();
	void DrawPlayerStats(PartyCharacter const &character, int i) const;
	void DrawSingleStat(PartyCharacter const& character, BaseStats stat, int x, int y) const;

	void DebugItems();
	void DebugQuests();

	void DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const;

	std::string PlayMapBgm(std::string name);
	void PlayDialogueSfx(std::string name);

	enum class MapState
	{
		NORMAL,
		ON_MESSAGE,
		ON_DIALOG,
		ON_MENU_SELECTION
	};

	bool godMode = false;
	bool statusOpen = false;

	std::random_device rd;
	std::uniform_int_distribution<> random100;

	std::string currentMap = "";

	EventTrigger tpInfo;

	std::unordered_set<int> currentSfxPlaying;

	Map map;
	Player player;
	GameParty* playerParty = nullptr;

	MapState state = MapState::NORMAL;

	pugi::xml_document currentDialogDocument;
	pugi::xml_node currentDialogNode;

	TransitionScene transitionTo = TransitionScene::NONE;

	const Window_Factory* windowFactory = nullptr;
	LookUpXMLNodeFromString xmlNode; //Maybe remove that when fixed?

	std::vector<std::unique_ptr<Window_Base>> pauseWindow;
	std::vector<std::unique_ptr<Window_Base>> statsWindow;

	int highDialogueSfx;
	int midDialogueSfx;
	int lowDialogueSfx;
	int battleStartSfx;
};


#endif __SCENE_MAP_H__
#ifndef __SCENE_MAP_H__
#define __SCENE_MAP_H__

#include "Scene_Base.h"
#include "GameParty.h"
#include "Player.h"
#include "Map.h"
#include "Map_Window_Menu.h"
#include "Map_Display_QuestLog.h"


#include <unordered_set>
#include <random>

class Scene_Map : public Scene_Base
{
public:
	Scene_Map() = default;
	explicit Scene_Map(GameParty* party);
	explicit Scene_Map(std::string const& newMap, GameParty* party);
	explicit Scene_Map(std::string const& newMap, iPoint playerCoords, GameParty* party);

	void Load(
		std::string const& path,
		LookUpXMLNodeFromString const& info,
		Window_Factory const& windowFac
	) override;
	
	bool isReady() override;

	void Start() override;
	TransitionScene Update() override;
	void Draw() override;

	int OnPause() override;

	iPoint GetTPCoordinates() const;
	int CheckNextScene() override;
	std::string_view GetNextMap() const;
	
	void SetPlayerParty(GameParty* party);
	void SpawnPlayerPosition();

	bool SaveScene(pugi::xml_node const&) override;
	bool LoadScene(pugi::xml_node const&) override;
	
	std::string_view GetFightName() const override;

	void DebugDraw() override;

	void SubscribeEventsToGlobalSwitches();

private:
	void DebugItems();
	void DebugQuests();
	void DebugInventory();
	void DebugAddALLItemsWithRandomAmounts();

	void DrawHPBar(int textureID, int currentHP, int maxHP, iPoint position) const;

	std::string PlayMapBgm(std::string_view name);
	void PlayDialogueSfx(std::string_view name);
	void DungeonSfx();

	TransitionScene TryRandomBattle();

	enum class MapState
	{
		NORMAL,
		ON_MESSAGE,
		ON_DIALOG,
		ON_MENU_SELECTION,
		ON_MENU
	};

	bool godMode = false;

	std::random_device rd;
	std::uniform_int_distribution<> random100;
	std::uniform_int_distribution<> random1000;

	std::string currentMap = "";

	EventTrigger tpInfo;

	Map map;
	Player player;
	GameParty* playerParty = nullptr;

	MapState state = MapState::NORMAL;
	MapState lastState = MapState::NORMAL;

	pugi::xml_document currentDialogDocument;
	pugi::xml_node currentDialogNode;

	TransitionScene transitionTo = TransitionScene::NONE;

	const Window_Factory* windowFactory = nullptr;
	LookUpXMLNodeFromString xmlNode; //Maybe remove that when fixed?

	std::vector<std::unique_ptr<Window_Base>> pauseWindow;
	std::unique_ptr<Map_Window_Menu> mainMenu;
	std::unique_ptr<Map_Display_QuestLog> questLog;

	int highDialogueSfx = -1;
	int midDialogueSfx = -1;
	int lowDialogueSfx = -1;
	int battleStartSfx = -1;
	int waterDropSfx = -1;
	int torchSfx = -1;
	int doorOpenSfx = -1;
	int dootCloseSfx = -1;
	int leverOpenSfx = -1;
	int levelCloseSfx = -1;

	iPoint spawnPlayerPosition = { 0, 0 };
	
	std::string_view nextFightName = "";
};


#endif //__SCENE_MAP_H__
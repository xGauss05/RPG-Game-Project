#ifndef __DB_ITEMS_H__
#define __DB_ITEMS_H__

#include <vector>

struct Item
{
	struct GeneralProperties
	{
		explicit GeneralProperties(pugi::xml_node const& node);

		std::string name = "";
		int iconIndex = 0;
		std::string description = "";
		int iTypeID = 1;
		int price = 0;
		bool consumable = true;
		int scope = 7;
		int ocasion = 0;
	};

	struct BattleProperties
	{
		explicit BattleProperties(pugi::xml_node const& node);

		int speed = 0;
		int successRate = 100;
		int repeats = 1;
		int hitType = 0;
		int animationID = 0;
	};

	struct DamageProperties
	{
		explicit DamageProperties(pugi::xml_node const& node);

		int type = 0;
		int elementID = 0;
		std::string damageFormula = "";
		int variance = 20;
		bool canCritical = false;
	};

	struct EffectProperties
	{
		explicit EffectProperties(pugi::xml_node const& node);

		int functionToCall = 11;
		int dataID = 0;
		float param1 = 0;
		float param2 = 0;
	};

	Item() = default;
	explicit Item(pugi::xml_node const& itemNode);

	int id = 0;
	GeneralProperties general;
	BattleProperties invocation;
	DamageProperties damage;
	EffectProperties effect;

};

class DB_Items
{
public:
	DB_Items();

	int GetItemID(std::string_view itemName) const;
	Item const &GetItem(int id) const;
	std::vector<Item> items;
};

#endif //__DB_ITEMS_H__

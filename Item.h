#pragma once
#include <array>
#include <map>
#include <string>

enum ItemCategory { None, Healing, Weapons, Farming, KeyItems };		//Could be used with a string instead
struct Item
{
	std::string name;
	ItemCategory category;
	double itemSellPrice;			//Could be used if trading is in the game
	int amount = 1;
	//std::string itemCategory;		//Could be replaced if not doing key items etc.
};

struct Item_v2
{
	ItemCategory category;
	double itemSellPrice;			//Could be used if trading is in the game
	int amount = 1;
};

typedef std::map<std::string, Item_v2> ItemList_v2;
//Looks up from Item item through strings
typedef std::map<std::string, std::vector<std::string>> ItemLookup_v2;
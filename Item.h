#pragma once
#include <array>
#include <map>
#include <string>
#include <vector>

enum ItemCategory { None, Healing, Weapons, Farming, KeyItems };		//Could be used with a string instead

struct Item_v2
{
	ItemCategory category;
	int itemSellPrice;			//Could be used if trading is in the game
};

typedef std::map<std::string, Item_v2> ItemMap;
// Items for enemies, traders
typedef std::vector<std::string> Items;

//Looks up from Item item through strings
typedef std::map<std::string, Items> ItemLookup;

//item name and quantity.  ++items["x"] adds one to 
typedef std::map<std::string, int> Inventory;

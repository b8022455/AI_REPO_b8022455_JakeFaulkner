#pragma once
#include <array>

enum ItemCategory { Healing, Weapons, Farming, KeyItems };		//Could be used with a string instead
struct Item
{
	std::string name;
	ItemCategory category;
	double itemSellPrice;			//Could be used if trading is in the game
	int amount = 1;
	//std::string itemCategory;		//Could be replaced if not doing key items etc.
};
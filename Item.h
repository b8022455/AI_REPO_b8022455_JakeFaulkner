#pragma once
#include <array>

enum ItemCategory { Healing, Weapons, Farming };		//Could be used with a string instead
struct Item
{
	std::string name;
	ItemCategory category;
	int amount = 1;
	//std::string itemCategory;		//Could be replaced if not doing key items etc.
};
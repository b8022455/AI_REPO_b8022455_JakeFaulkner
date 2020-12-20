#pragma once
#include <utility>
#include "Constants.h"
#include "GameObject.h"

// NPCS
// todo inherit from gameobject, change init, add gameobject override functions
class Trader // : public GameObject
{
	const InventoryUnordered* mRequestItems;
	const InventoryUnordered* mRewardItems;
	// Swap when traded
	DialogPair mDialog;
	
	bool traded = false;
public:

	Trader(const std::string& dialog, const std::string& requestList, const std::string& rewardItems);

	bool CanTrade()
	{
		return !traded;
	}
	// Adds rewards to parameter inventory. Returns false if already traded
	bool GiveRewards(Inventory& inventory)
	{
		if (CanTrade())
		{
			// adds items
			for (auto& i : *mRewardItems)
			{
				inventory[i.first] += i.second;
			}

			// change state to reflect trader completed
			traded = true;
			// swap dialog to avoid logic

			mDialog.first.swap(mDialog.second);

			return true;
		}
		else
		{
			// Already traded
			return false;
		}
	}

	// for player to search for in inventory
	const InventoryUnordered* GetRequestItems()
	{
		return mRequestItems;
	}

	// Returns dialog before and after trading.
	std::string GetDialog()
	{
		return mDialog.first;
	}



};


#pragma once
#include "Constants.h"
#include "GameObject.h"
// NPCS
// todo inherit from gameobject, change init, add gameobject override functions
class Trader // : public GameObject
{
	const Items* mRequestItems;
	const Items* mRewardItems;
	// Swap when traded
	std::array<std::string, 2> mDialog; 

	bool traded = false;
public:

	Trader(const std::string& requestList, const std::string& rewardItems);
	

	// Returns item pointers container
	const Items* GiveRewards()
	{
		if (!traded)
		{
			traded = true;
			// swap dialog to avoid logic
			mDialog.front().swap(mDialog.back());
			return mRewardItems;
		}
		else
		{
			// Already traded
			return nullptr;
		}
	}

	// for player to search for in inventory
	const Items* GetRequestItems()
	{
		return mRequestItems;
	}

	// Returns dialog before and after trading.
	std::string GetDialog()
	{
		return mDialog.front();
	}



};


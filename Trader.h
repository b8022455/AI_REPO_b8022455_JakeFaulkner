#pragma once
#include "Constants.h"
#include "GameObject.h"
// NPCS
// todo inherit from gameobject, change init, add gameobject override functions
class Trader // : public GameObject
{
	ItemListPointers mRequestItems;
	ItemListPointers mRewardItems;
	// Swap when traded
	std::array<std::string, 2> mDialog; 

	bool traded = false;
public:

	Trader(const ItemListPointers& request, const ItemListPointers& reward)
		:
		mRequestItems(request),
		mRewardItems(reward)
	{

	}

	// Returns item pointers container
	ItemListPointers* GiveRewards()
	{
		if (!traded)
		{
			traded = true;
			// swap dialog to avoid logic
			mDialog.front().swap(mDialog.back());
			return &mRewardItems;
		}
		else
		{
			// Already traded
			return nullptr;
		}
	}

	// for player to search for in inventory
	ItemListPointers* GetRequestItems()
	{
		return &mRequestItems;
	}

	// Returns dialog before and after trading.
	std::string GetDialog()
	{
		return mDialog.front();
	}



};


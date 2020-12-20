#include "Trader.h"
#include "Constants.h"


Trader::Trader(const std::string& dialog,const std::string & requestList, const std::string & rewardItems)
	:
	mRequestItems(&GC::ITEM_LOOKUP_REQUEST.at(requestList)),
	mRewardItems(&GC::ITEM_LOOKUP_REWARD.at(rewardItems)),
	mDialog(GC::DIALOG_PAIR.at(dialog))
{
	
}


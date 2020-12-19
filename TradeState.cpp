#include "TradeState.h"
#include "PlayState.h"
#include "Constants.h"
#include "GameApp.h"

using ButtonState = GamePad::ButtonStateTracker::ButtonState;

TradeState::TradeState()
{
}

void TradeState::Update(const GameTimer & gt)
{
	{
		if (mpTrader)
		{
			GameApp::Get().mDebugLog << "Traded? " << !mpTrader->CanTrade() << "\n\n";
		}


		std::for_each(mpInventory->begin(), mpInventory->end(), [&](auto& inv)
		{
			GameApp::Get().mDebugLog << inv.first << " : " << inv.second << "\n";
		});

		if (*mpInventoryPosition != mpInventory->end())
			GameApp::Get().mDebugLog << "Current Selected Item: " << (*mpInventoryPosition)->first << " x" << (*mpInventoryPosition)->second << "\n";
	}

	if (InputUp()) // W
	{
		
	}

	if (InputLeft()) // A
	{
		
	}

	if (InputRight()) // D
	{
		if (Trade())
		{
			// todo play success noise
		}
		else
		{
			// todo play fail noise
		}
	}

	if (InputDown()) // S
	{
		// return to game
		GameApp::Get().ChangeState(GC::STATE_PLAY);
	}
}

void TradeState::Draw(const GameTimer & gt)
{
}

void TradeState::OnResume()
{
	PlayState* pPlayState((PlayState*)GameApp::Get().GetState(GC::STATE_PLAY));
	assert(pPlayState);

	mpPlayer = pPlayState->GetPlayer();
	mpTrader = pPlayState->GetTrader();
	mpInventory = pPlayState->GetInventory();
	mpInventoryPosition = pPlayState->GetInventoryPos();
	// if any one element is not present then go back to PlayState
	if (!mpTrader || !mpPlayer || !mpInventory)
	{
		assert(false);
		GameApp::Get().ChangeState(GC::STATE_PLAY);
	}

}

bool TradeState::InputUp()
{
	return
		Input::Get().KeyHeld(GC::KEY_FW) ||
		Input::Get().GamePad().dpadUp == ButtonState::RELEASED ||
		Input::Get().GamePad().y == ButtonState::RELEASED;
}

bool TradeState::InputDown()
{
	return Input::Get().KeyHeld(GC::KEY_BK) ||
		Input::Get().GamePad().dpadDown == ButtonState::RELEASED ||
		Input::Get().GamePad().a == ButtonState::RELEASED;
}

bool TradeState::InputRight()
{
	return Input::Get().KeyHeld(GC::KEY_RT) ||
		Input::Get().GamePad().dpadRight == ButtonState::RELEASED ||
		Input::Get().GamePad().b == ButtonState::RELEASED;
}

bool TradeState::InputLeft()
{
	return Input::Get().KeyHeld(GC::KEY_LT) ||
		Input::Get().GamePad().dpadLeft == ButtonState::RELEASED ||
		Input::Get().GamePad().x == ButtonState::RELEASED;
}

void TradeState::CleanInventory(Inventory& inv)
{
	for (Inventory::iterator it = inv.begin(); it != inv.end();)
	{
		if ((*it).second <= 0)
		{
			it = inv.erase(it);
		}
		else 
		{
			++it;
		}
	}
}

bool TradeState::Trade()
{

	Inventory::iterator tempIt = *mpInventoryPosition;
	
	const InventoryUnordered* request = mpTrader->GetRequestItems();

	// copy to manipulate for trade
	Inventory temp = *mpInventory;

	bool validTrader = true;
	bool ret = false;

	// start at top of request
	InventoryUnordered::const_iterator rIt = request->begin();

	// evaluates if trade can be done and removes items
	while (rIt != request->end() && validTrader)
	{
		// request items minused from temp inventory.
		// If item isn't there, it is created. the trade will fail so temp inventory will be discarded
		temp[rIt->first] -= rIt->second;

		// no trade if not enough of an item int the traders request
		if (temp.at(rIt->first) < 0)
		{
			validTrader = false;
		}

		++rIt;
	}

	if (validTrader)
	{
		//add items to temp
		if (mpTrader->GiveRewards(temp))
		{
			CleanInventory(temp); // remove anything that the inventory has nothing of

			__int64 distance = std::distance(mpInventory->begin(), tempIt); // location of current itterator
			__int64 difference = mpInventory->size() - temp.size();

			// save changes to inventory
			(*mpInventory) = std::move(temp);

			// if temp is smaller minus difference
			if (difference > 0)
			{
				distance -= difference;
			}

			// reset iterator since inventory ahs changed
			tempIt = mpInventory->begin();
			// go to location
			std::advance(tempIt, distance);

			(*mpInventoryPosition) = std::move(tempIt);

			ret = true;
		}
	}

	return ret;

}

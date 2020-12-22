#include "TradeState.h"
#include "PlayState.h"
#include "Constants.h"
#include "GameApp.h"

using ButtonState = GamePad::ButtonStateTracker::ButtonState;

TradeState::TradeState()
{
}

void TradeState::Initialize()
{
	const DirectX::XMINT2 s = GameApp::Get().GetClientSizeU2(); //todo on resize


	const float textPadding = 128.0f;


	const long top = s.y - (s.y / 2);
	const long colTop = top + 128;
	const long colDivX = s.x / 2;

	const float topStartFloat = (float)top;
	const float colTopFloat = (float)top;
	const float colDivXFloat = (float)colDivX;

	//const float offset = 32.0f;
	//const float topOffset = offset + top;
	//const float right = (0.5f*s.y) + offset + offset;

	//mBackground.sourceRectangle = { 0, top ,s.x,s.y };
	//mBackground.position = { 0,top };

	mBackground.destinationRectangle = { 0, top, s.x, s.y };
	mBackground.Initialise("iceTex");

	
	mText[TEXT_TRADER].position = { textPadding ,topStartFloat };
	// left side
	mText[TEXT_REQUEST].position = { textPadding , colTopFloat };
	// right side
	mText[TEXT_REWARD].position = { colDivXFloat + textPadding, colTopFloat }; // middle

	mText[TEXT_TRADER].fontIndex = 1;
	mText[TEXT_REQUEST].fontIndex = 1;
	mText[TEXT_REWARD].fontIndex = 1;



	const RECT r{
		GC::PANEL_SRC[0],
		GC::PANEL_SRC[1],
		GC::PANEL_SRC[2],
		GC::PANEL_SRC[3],
	};

	mPanels.at(PANEL_TOP).Initialize("uiTex", r, { 0, top, s.x, colTop });

	mPanels.at(PANEL_LEFT).Initialize("uiTex", r, { 0,colTop,colDivX,s.y });

	mPanels.at(PANEL_RIGHT).Initialize("uiTex", r, { colDivX,colTop,s.x,s.y });
}

void TradeState::Update(const GameTimer & gt)
{
	{

		//show inventory debug
		GameApp::Get().mDebugLog << "\nInventory:\n";

		std::for_each(mpInventory->begin(), mpInventory->end(), [&](auto& inv)
		{
			GameApp::Get().mDebugLog << inv.first << " : " << inv.second << "\n";
		});

		if (*mpInventoryPosition != mpInventory->end())
			GameApp::Get().mDebugLog << "Current Selected Item: " << (*mpInventoryPosition)->first << " x" << (*mpInventoryPosition)->second << "\n";
	}

	if (Input::Get().AnyMenuButtonPressed())
	{
		GameApp::Get().PlayClickDownAudio();
	}

	if (Input::Get().MenuInputUp()) // W
	{
	}

	if (Input::Get().MenuInputLeft()) // A
	{
	}

	if (Input::Get().MenuInputRight()) // D
	{
		if (Trade())
		{
			RefreshText(); // update text
			GameApp::Get().PlayClickUpAudio(true);
		}
		else
		{
			GameApp::Get().PlayClickUpAudio(false);
		}
	}

	if (Input::Get().MenuInputDown()) // S
	{
		GameApp::Get().PlayClickUpAudio(true);

		// return to game
		GameApp::Get().ChangeState(GC::STATE_PLAY);
	}
}

void TradeState::Draw(const GameTimer & gt)
{
	mBackground.Draw();

	for (auto& p : mPanels)
	{
		p.Draw();
	}
	for (auto& t : mText)
	{
		t.Draw();
	}

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
	else
	{
		RefreshText();
	}


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

void TradeState::RefreshText()
{
	
	mText[TEXT_TRADER].string = mpTrader->GetDialog().c_str();

	if (mpTrader->CanTrade())
	{
		// show trade items if can trade
		mText[TEXT_REQUEST].string = mpTrader->GetRequestAsString().c_str();
		mText[TEXT_REWARD].string = mpTrader->GetRewardAsString().c_str();
	}
	else
	{
		//show nothing
		mText[TEXT_REQUEST].string = "";
		mText[TEXT_REWARD].string = mText[TEXT_REQUEST].string;
	}
}

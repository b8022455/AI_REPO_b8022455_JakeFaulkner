#include "TradeState.h"
#include "PlayState.h"
#include "Constants.h"
#include "GameApp.h"

TradeState::TradeState(const Button & b0, const Button & b1, const Button & b2, const Button & b3)
	:
	mButtons{b0,b1,b2,b3}
{
}

void TradeState::Update(const GameTimer & gt)
{

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
	// if any one element is not present then go back to PlayState
	if (!mpTrader || !mpPlayer || !mpInventory)
	{
		assert(false);
		GameApp::Get().ChangeState(GC::STATE_PLAY);
	}

}

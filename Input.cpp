//#include "Input.h"
#include "Common/d3dApp.h"
#include <vector>
#include "GameApp.h"



void Input::Initialize()
{
	mGamepad = std::make_unique<DirectX::GamePad>();
	mKeyboard = std::make_unique<DirectX::Keyboard>();


	
}

void Input::Update()
{
	//Keyboard state and tracker
	mKeyState = mKeyboard->GetState();
	mKeyTracker.Update(mKeyState);

	// Gamepad state and tracker
	mGamePadState = mGamepad->GetState(mGamePadPlayer);
	if (mGamePadState.IsConnected())
	{
		mGamePadTracker.Update(mGamePadState);
	}
}

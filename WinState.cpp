#include "WinState.h"
#include "GameApp.h"

WinState::WinState(const Text& t1)
{
	mTitle = t1;

	DirectX::XMFLOAT2 centre = GameApp::Get().GetClientSize();
	centre.x *= 0.5f;
	centre.y *= 0.6f;
}

void WinState::Initialize()
{

}

void WinState::Update(const GameTimer& gt)
{
	if (Input::Get().AnyMenuButtonPressed())
		GameApp::Get().PlayClickDownAudio();

	//if(Input::Get().MenuUpHeld())


}

void WinState::Draw(const GameTimer& gt)
{
	mTitle.Draw();
}

void WinState::OnResume()
{

}
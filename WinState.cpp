#include "WinState.h"
#include "GameApp.h"

WinState::WinState(const Text& t1, const Text& t2, const Button& btn)
{
	mTitle = t1;
	mDescription = t2;
	mBtn = btn;

	DirectX::XMFLOAT2 centre = GameApp::Get().GetClientSize();
	centre.x *= 0.5f;
	centre.y *= 0.6f;

	mBtn.SetPos({ centre.x, centre.y });
}

void WinState::Initialize()
{

}

void WinState::Update(const GameTimer& gt)
{
	if (Input::Get().AnyMenuButtonPressed())
		GameApp::Get().PlayClickDownAudio();

	if (Input::Get().AnyMenuButtonPressed())
		mBtn.SetColor(GC::BUTTON_DOWN_COLOR);

	if (Input::Get().MenuInputUp())		//Go back to start menu
	{
		GameApp::Get().PlayClickUpAudio(true);
		mBtn.SetColor(GC::BUTTON_UP_COLOR);
		mBtn.Activate();
	}

}

void WinState::Draw(const GameTimer& gt)
{
	mTitle.Draw();
	mDescription.Draw();
}

void WinState::OnResume()
{

}
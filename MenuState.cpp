#include "MenuState.h"
#include "GameApp.h"
#include "Constants.h"

using ButtonState = GamePad::ButtonStateTracker::ButtonState;


MenuState::MenuState(const Button & b0, const Button & b1, const Button & b2, const Button & b3)
{
	mButtons.reserve(4);

	mButtons.push_back(b0);
	mButtons.push_back(b1);
	mButtons.push_back(b2);
	mButtons.push_back(b3);

	DirectX::XMFLOAT2 centre = GameApp::Get().GetClientSize();
	centre.x *= 0.5f;
	centre.y *= 0.6f;

	const DirectX::XMFLOAT2 offset = { 200.0f , 100.0f};

	mButtons.at(0).SetPos({ centre.x			,	centre.y - offset.y		});
	mButtons.at(1).SetPos({ centre.x - offset.x	,	centre.y				});
	mButtons.at(2).SetPos({ centre.x + offset.x	,	centre.y				});
	mButtons.at(3).SetPos({ centre.x			,	centre.y + offset.y		});
}

void MenuState::Update(const GameTimer & gt)
{ 
	assert(mButtons.size() == 4);

	if (Input::Get().MenuButtonPressed())
	{
		GameApp::Get().PlayClickDownAudio();
	}

	//todo on release

	if (Input::Get().MenuInputUp()) // W
	{
		GameApp::Get().PlayClickUpAudio(true);
		mButtons.at(0).Activate();
	}

	if (Input::Get().MenuInputLeft()) // A
	{
		GameApp::Get().PlayClickUpAudio(true);
		mButtons.at(1).Activate();
	}
	
	if (Input::Get().MenuInputRight()) // D
	{
		GameApp::Get().PlayClickUpAudio(true);
		mButtons.at(2).Activate();
	}
	
	if (Input::Get().MenuInputDown()) // S
	{
		GameApp::Get().PlayClickUpAudio(true);
		mButtons.at(3).Activate();
	}
}

void MenuState::Draw(const GameTimer & gt)
{
	for (auto& b : mButtons)
	{
		b.Draw();
	}
}
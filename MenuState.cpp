#include "MenuState.h"
#include "GameApp.h"
#include "Constants.h"

using ButtonState = GamePad::ButtonStateTracker::ButtonState;


MenuState::MenuState(const Text & t, const Text & b, const Button & b0, const Button & b1, const Button & b2, const Button & b3)
{
	DirectX::SimpleMath::Vector2 clientSize = GameApp::Get().GetClientSize();

	mTitle = t;
	mBody = b;

	mButtons.reserve(4);

	mButtons.push_back(b0);
	mButtons.push_back(b1);
	mButtons.push_back(b2);
	mButtons.push_back(b3);


	DirectX::XMFLOAT2 relButtonPivot = clientSize * GC::MENU_BUTTON_PIVOT;

	mButtons.at(0).SetPos({ relButtonPivot.x								,	relButtonPivot.y - GC::MENU_BUTTON_DISTANCE.y });
	mButtons.at(1).SetPos({ relButtonPivot.x - GC::MENU_BUTTON_DISTANCE.x	,	relButtonPivot.y });
	mButtons.at(2).SetPos({ relButtonPivot.x + GC::MENU_BUTTON_DISTANCE.x	,	relButtonPivot.y });
	mButtons.at(3).SetPos({ relButtonPivot.x								,	relButtonPivot.y + GC::MENU_BUTTON_DISTANCE.y });
}

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

void MenuState::Initialize()
{
	
}

void MenuState::Update(const GameTimer & gt)
{ 
	assert(mButtons.size() == 4);

	if (Input::Get().AnyMenuButtonPressed())
	{
		GameApp::Get().PlayClickDownAudio();
	}

	//down
	if (Input::Get().MenuUpHeld()) mButtons.at(0).SetColor(GC::BUTTON_DOWN_COLOR);
	if(Input::Get().MenuLeftHeld()) mButtons.at(1).SetColor(GC::BUTTON_DOWN_COLOR);
	if(Input::Get().MenuRightHeld())mButtons.at(2).SetColor(GC::BUTTON_DOWN_COLOR);
	if(Input::Get().MenuDownHeld())mButtons.at(3).SetColor(GC::BUTTON_DOWN_COLOR);

	//Release

	if (Input::Get().MenuInputUp()) // W
	{
		GameApp::Get().PlayClickUpAudio(true);
		mButtons.at(3).SetColor(GC::BUTTON_UP_COLOR);
		mButtons.at(0).Activate();
	}

	if (Input::Get().MenuInputLeft()) // A
	{
		GameApp::Get().PlayClickUpAudio(true);
		mButtons.at(3).SetColor(GC::BUTTON_UP_COLOR);
		mButtons.at(1).Activate();
	}
	
	if (Input::Get().MenuInputRight()) // D
	{
		GameApp::Get().PlayClickUpAudio(true);
		mButtons.at(3).SetColor(GC::BUTTON_UP_COLOR);
		mButtons.at(2).Activate();
	}
	
	if (Input::Get().MenuInputDown()) // S
	{
		GameApp::Get().PlayClickUpAudio(true);
		mButtons.at(3).SetColor(GC::BUTTON_UP_COLOR);
		mButtons.at(3).Activate();
	}
}

void MenuState::Draw(const GameTimer & gt)
{
	mTitle.Draw();
	mBody.Draw();
	//mTempPanel.Draw();

	for (auto& b : mButtons)
	{
		b.Draw();
	}
}
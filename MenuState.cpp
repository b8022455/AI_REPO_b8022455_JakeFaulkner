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

	//todo on release

	if (InputUp()) // W
	{
		mButtons.at(0).Activate();
	}

	if (InputLeft()) // A
	{
		mButtons.at(1).Activate();
	}
	
	if (InputRight()) // D
	{
		mButtons.at(2).Activate();
	}
	
	if (InputDown()) // S
	{
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

bool MenuState::InputUp()
{
	return 
		Input::Get().KeyHeld(GC::KEY_FW) || 
		Input::Get().GamePad().dpadUp == ButtonState::RELEASED || 
		Input::Get().GamePad().y == ButtonState::RELEASED;
}

bool MenuState::InputDown()
{
	return Input::Get().KeyHeld(GC::KEY_BK) ||
		Input::Get().GamePad().dpadDown == ButtonState::RELEASED ||
		Input::Get().GamePad().a == ButtonState::RELEASED;
}

bool MenuState::InputRight()
{
	return Input::Get().KeyHeld(GC::KEY_RT) ||
		Input::Get().GamePad().dpadRight == ButtonState::RELEASED ||
		Input::Get().GamePad().b == ButtonState::RELEASED;
}

bool MenuState::InputLeft()
{
	return Input::Get().KeyHeld(GC::KEY_LT) ||
		Input::Get().GamePad().dpadLeft == ButtonState::RELEASED ||
		Input::Get().GamePad().x == ButtonState::RELEASED;
}

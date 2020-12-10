#include "MenuState.h"
#include "GameApp.h"

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

	if (GetAsyncKeyState('W') & 0x8000) // W
	{
		mButtons.at(0).Activate();
	}

	if (GetAsyncKeyState('A') & 0x8000) // A
	{
		mButtons.at(1).Activate();
	}
	
	if (GetAsyncKeyState('D') & 0x8000) // D
	{
		mButtons.at(2).Activate();
	}
	
	if (GetAsyncKeyState('S') & 0x8000) // S
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

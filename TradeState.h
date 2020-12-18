#pragma once
#include "State.h"
#include "SpriteManager.h"
#include <vector>
//

class Player;
class Trader;
class Item;

class TradeState : public State
{
	std::vector<Button> mButtons;
	// from playstate on resume
	Player* mpPlayer = nullptr;
	//from playstate on resume
	Trader* mpTrader = nullptr;

	std::vector<Item>* mpInventory;

public:
	TradeState(const Button& b0, const Button& b1, const Button& b2, const Button& b3);

	virtual void Initialize()								override{};
	virtual void Update(const GameTimer& gt)				override;
	virtual void Draw(const GameTimer& gt)					override;
	virtual void OnResume()									override;


	virtual void OnMouseDown(WPARAM btnState, int x, int y) override {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override {};
	virtual void OnKeyboardInput(const GameTimer& gt)		override {};
	bool InputUp();
	bool InputDown();
	bool InputRight();
	bool InputLeft();
};


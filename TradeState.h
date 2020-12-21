#pragma once
#include "State.h"
#include "SpriteManager.h"
#include <vector>
#include "Item.h"
//

class Player;
class Trader;
class Item;


class TradeState : public State
{
	// from playstate on resume
	Player* mpPlayer = nullptr;
	//from playstate on resume
	Trader* mpTrader = nullptr;

	Inventory* mpInventory = nullptr;
	Inventory::iterator* mpInventoryPosition;

public:
	TradeState();

	virtual void Initialize()								override {};
	virtual void Update(const GameTimer& gt)				override;
	virtual void Draw(const GameTimer& gt)					override;
	virtual void OnResume()									override;


	virtual void OnMouseDown(WPARAM btnState, int x, int y) override {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override {};
	virtual void OnKeyboardInput(const GameTimer& gt)		override {};
	void CleanInventory(Inventory& inv);
	bool Trade();
};


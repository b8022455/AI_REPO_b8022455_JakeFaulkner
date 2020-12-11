#pragma once
#include "State.h"
#include "SpriteManager.h"

class MenuState : public State
{
	std::vector<Button> mButtons;
public:
	MenuState(const Button& b0, const Button& b1, const Button& b2, const Button& b3);

	virtual void Initialize() override {};
	virtual void Update(const GameTimer& gt) override ;
	virtual void Draw(const GameTimer& gt) override ;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override{};
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override{};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override{};
	virtual void OnKeyboardInput(const GameTimer& gt)		override{};
};




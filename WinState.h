#pragma once
#include "State.h"

class WinState : public State
{
public:
	WinState(const Text& t1);

	virtual void Initialize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
	virtual void OnResume() override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override {};
	virtual void OnKeyboardInput(const GameTimer& gt) override {};

private:
	Text mTitle;
};
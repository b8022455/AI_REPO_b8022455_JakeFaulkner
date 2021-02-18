#pragma once
#include "State.h"
#include "SpriteManager.h"

class TutorialDialogState :
	public State
{
private:
	std::string& mDialog;
	Panel mPanel;
	Text mText;
public:
	virtual void Initialize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
	virtual void OnResume() override;
	virtual void OnPause() override;
	virtual void Reset() override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override {};
	virtual void OnKeyboardInput(const GameTimer& gt) override {};
};


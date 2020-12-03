#pragma once
#include "State.h"
//#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "CombatController.h"
#include "Common/Camera.h"


class PlayState : public State
{
	GameObject mGameObject;
	Player mPlayer;
	Enemy mEnemy;
	CombatController mCombatController;
	Camera mCamera;

	virtual void Initialize() override;
	virtual void Update(const GameTimer& gt) override;
	//virtual void Draw(const GameTimer& gt) override; // todo not needed

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual void OnKeyboardInput(const GameTimer& gt)		override;


	//todo move to constants header in GC namespace
	const float RIGHTBOUND = 15.0f;
	const float LEFTBOUND = -RIGHTBOUND;
	const float YBOUNDLOW = 50.0f;
	const float YBOUNDHIGH = 300.0f;
	const float UPBOUND = 15.0f;
	const float DOWNBOUND = -UPBOUND;
};


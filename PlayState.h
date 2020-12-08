#pragma once
#include "State.h"
//#include "GameObject.h"
#include "CombatController.h"
#include "Common/Camera.h"
#include "Tile.h"
using namespace std;

class PlayState : public State
{
	Player mPlayer;
	PlayerWeapon mPlayerWeapon;
	std::vector<Enemy> mEnemies;
	Tile mTile;
	TileManager mTileManager;
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


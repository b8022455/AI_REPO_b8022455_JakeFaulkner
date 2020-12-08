#pragma once
#include "State.h"
//#include "GameObject.h"
#include "CombatController.h"
#include "Common/Camera.h"
#include "Tile.h"
#include "SpriteManager.h"


class PlayState : public State
{
	Player mPlayer;
	PlayerWeapon mPlayerWeapon;
	std::vector<Enemy> mEnemies;

	TileManager mTileManager;
	CombatController mCombatController;
	Camera mCamera;
	std::unordered_map<std::string,Sprite> mSprites;

	virtual void Initialize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

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


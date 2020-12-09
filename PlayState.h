#pragma once
#include "State.h"
//#include "GameObject.h"
#include "CombatController.h"
#include "Common/Camera.h"
#include "Tile.h"
#include <array>


class PlayState : public State
{
	//Tile mTile;
	
	Player mPlayer;
	PlayerWeapon mPlayerWeapon;
	std::vector<Enemy> mEnemies;

	TileManager mTileManager;
	CombatController mCombatController;

	enum CAMERA_TYPE
	{
		GAME = 0,
		DEBUG = 1,
		SIZE //Always last
	};

	CAMERA_TYPE mCamType = CAMERA_TYPE::GAME;

	std::array<Camera, CAMERA_TYPE::SIZE> mCameras;
	//Camera mCamera;
	//Camera mCameraDebug;
	

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

	const float CAM_ANGLE = 45.0f;
	const DirectX::XMFLOAT3 CAM_OFFSET{ 0.0f,10.0f,-10.0f }; 
};


#pragma once
#include "State.h"
//#include "GameObject.h"
#include "CombatController.h"
#include "Common/Camera.h"
#include "Tile.h"
#include "Item.h"
#include <array>
#include "SpriteManager.h"
#include <unordered_map>
#include "Experience.h"
#include "Constants.h"
#include "Trader.h"


class PlayState : public State
{
	Player mPlayer;
	PlayerWeapon mPlayerWeapon;
	std::vector<Enemy> mEnemies;
	std::vector<Item> Inventory;
	Trader mTempTrader;
	// Trader player is focused on
	Trader* mpActiveTrader = nullptr;
	int inventoryPosition = 0;			//Used to select items from the Inventory (Increments using down key, decrements using up key, loops back around when over inventory size)
	bool itemMenuOpen = false;			//Debug: only allow item selection when on a menu(either pause or an item menu)

	TileManager mTileManager;
	Tile mTile;
	CombatController mCombatController;
	std::unordered_map<std::string, Sprite> mSprites;

	enum CAMERA_TYPE
	{
		GAME = 0,
		DEBUG = 1,
		SIZE //Always last
	};

	CAMERA_TYPE mCamType = CAMERA_TYPE::GAME;

	std::array<Camera, CAMERA_TYPE::SIZE> mCameras;
public:
	PlayState();

	virtual void Initialize() override;
	virtual void Update(const GameTimer& gt)				override;
	virtual void Draw(const GameTimer& gt)					override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual void OnKeyboardInput(const GameTimer& gt)		override;
	virtual void OnResume()									override{};


	void Controls(const GameTimer& gt);

	Player* GetPlayer()
	{
		return &mPlayer;
	}

	Trader* GetTrader()
	{
		return mpActiveTrader;
	}

	std::vector<Item>* GetInventory()
	{
		return &Inventory;
	}
	
	//todo move to constants header in GC namespace
	const float RIGHTBOUND = 15.0f;
	const float LEFTBOUND = -RIGHTBOUND;
	const float YBOUNDLOW = 50.0f;
	const float YBOUNDHIGH = 300.0f;
	const float UPBOUND = 15.0f;
	const float DOWNBOUND = -UPBOUND;

	const float CAM_ANGLE = 45.0f;
	const DirectX::XMFLOAT3 CAM_OFFSET{ 0.0f,10.0f,-10.0f }; 
	Experience mExperience;

};


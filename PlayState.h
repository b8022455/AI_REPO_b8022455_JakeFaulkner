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
	std::vector<Trader> mTraders;
	std::vector<Enemy> mEnemies;
	Inventory mInventory;
	Trader mTempTrader;


	// Trader player is focused on
	Trader* mpActiveTrader = nullptr;
	Inventory::iterator inventoryPosition = mInventory.begin();			//Used to select items from the Inventory (Increments using down key, decrements using up key, loops back around when over inventory size)
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

	void InitializeTraders();
	bool FindNearestTraderInRadius();
public:
	PlayState();

	virtual void Initialize() override;
	virtual void Update(const GameTimer& gt)				override;
	virtual void Draw(const GameTimer& gt)					override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)	override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual void OnKeyboardInput(const GameTimer& gt)		override;
	virtual void OnResume()									override;


	void Controls(const GameTimer& gt);

	Player* GetPlayer()
	{
		return &mPlayer;
	}

	Trader* GetTrader()
	{
		return mpActiveTrader;
	}

	Inventory* GetInventory()
	{
		return &mInventory;
	}

	Inventory::iterator* GetInventoryPos()
	{
		return &inventoryPosition;
	}

	void InventoryUp();
	void InventoryDown();
	void ItemAction();


	// Only use after changes have been made of inventory. Removes items with 0 quantity.
	void CleanInventory(Inventory& inv);

	const float CAM_ANGLE = 45.0f;
	const DirectX::XMFLOAT3 CAM_OFFSET{ 0.0f,10.0f,-10.0f }; 
	Experience mExperience;

};


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
#include "Plant.h"
#include <math.h>
#include "Bar.h"
using namespace std;

class PlayState : public State
{
	Player mPlayer;
	PlayerWeapon mPlayerWeapon;
	std::vector<Trader> mTraders;
	std::vector<Enemy> mEnemies;
	std::vector<Plant> mPlants;
	Inventory mInventory;

	// Trader player is focused on
	Trader* mpActiveTrader = nullptr;
	Inventory::iterator inventoryPosition = mInventory.begin();			//Used to select items from the Inventory (Increments using down key, decrements using up key, loops back around when over inventory size)
	bool itemMenuOpen = false;				//Debug: only allow item selection when on a menu(either pause or an item menu)

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

	Bar mPlayerHealthBar;

	Panel mInventoryPanel;
	// swaps on toggle and lerps on update
	std::pair<SimpleMath::Vector2, SimpleMath::Vector2> mInventoryLocation;
	Text mInventoryText;

	Text mScoreText;
	Text mScoreTextShadow;

	std::pair<SimpleMath::Vector2, SimpleMath::Vector2> mHelpLocation;
	Panel mHelpPanel;
	Text mHelpText;
	// "Level Up", "You can't go there"
	FadeText mMessage;

	FadeText mHelpMessage;
	FadeText mTradeHelpMessage;
	DeltaTimer revolvingHintTimer;
	int revolvingHintPosition = 3;

	void InitializeTraders();
	bool FindNearestTraderInRadius();
	void StoreScore();
	void GetName();

	int score = 0;
	std::string playerName = "";

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

	void reInitialize();

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

	const float GetRandomVoicePitch() const
	{
		return GC::VOICE_PITCH[rand()%GC::VOICE_PITCH_COUNT];
	}

	const float GetRandomFootstepPitch() const
	{
		return GC::FOOTSTEP_PITCH[rand() % GC::FOOTSTEP_PITCH_COUNT];
	}

	const float CAM_ANGLE = 45.0f;
	const DirectX::XMFLOAT3 CAM_OFFSET{ 0.0f,10.0f,-10.0f }; 
	Experience mExperience;

	// todo Sam/Jake description here please
	void ReGen();
	void ResetState(const GameTimer& gt);

	void UiUpdate(const GameTimer& gt);
	// Returns if planted successful
	bool CreatePlant();
	void UpdatePlayer(const GameTimer& gt);
	void UpdatePlants(const GameTimer& gt);
	void UpdateTiles(const GameTimer& gt);
	void UpdateEnemies(const GameTimer& gt);
	void HarvestByRadius();

	void Keyboard(const GameTimer& gt);
	void KeyboardDebug(const GameTimer& gt);
	void Gamepad(const GameTimer& gt);

	bool TraderStoryComplete();

	int EnemiesRemaining()
	{
		int count = 0;
		for (auto& e : mEnemies)
		{
			if (e.mEnabled) ++count;
		}
		return count;
	}
};


#include "XmfloatOverload.h"
#include "PlayState.h"
#include "GameApp.h"
#include "XmfloatOverload.h"
#include "SimpleMath.h"
//#include "Input.h"
#include "Constants.h"
#include <unordered_map>
#include <algorithm>
#include <iomanip>

using ButtonState = GamePad::ButtonStateTracker::ButtonState;

void PlayState::InitializeBuildings()
{
	mBuildings.reserve(3);

	mBuildings.push_back(Building(GC::BUILDING_0));
	mBuildings.push_back(Building(GC::BUILDING_1));
	mBuildings.push_back(Building(GC::BUILDING_2));
	
	struct SetupBuildings
	{
		float x = -5.5f;
		const int biggestGap = 5;
		void operator()(Building& b)
		{
			b.Initialize(GC::GO_BUILDING); // no model linked yet
			b.mpInstance->MaterialIndex = 0;
			x += 1.0f + (rand() % biggestGap);
			b.SetPos({ x, 0.0f, 4.0f });
		}
	};

	std::for_each(mBuildings.begin(), mBuildings.end(), SetupBuildings());
}

bool PlayState::FindNearestBuildingInRadius()
{
	struct SortByDistance
	{
		SimpleMath::Vector3 from;

		bool operator()(const Building& a, const Building& b)
		{
			distanceA = SimpleMath::Vector3::Distance(from, a.GetPosV3());
			distanceB = SimpleMath::Vector3::Distance(from, b.GetPosV3());

			return distanceA < distanceB;
		}
	private:
		float distanceA, distanceB;
	};

	SortByDistance s;
	// distance from player location
	s.from = mPlayer.GetPosV3();

	std::sort(mBuildings.begin(), mBuildings.end(), std::ref(s));

	float distance = SimpleMath::Vector3::Distance(mPlayer.GetPosV3(), mBuildings.front().GetPosV3());

	if (distance < GC::BUILDING_RADIUS)
	{
		mpActiveBuilding = &mBuildings.front();
		return true;
	}
	else
	{
		// if none in range clear any active building
		mpActiveBuilding = nullptr;
		return false;
	}
}

void PlayState::InitializeTraders()
{

	mTraders.reserve(10);
	
	mTraders.push_back(Trader(GC::TRADER_NAME_TEST, GC::TRADER_NAME_TEST, GC::TRADER_NAME_TEST));
	mTraders.push_back(Trader(GC::TRADER_NAME_1, GC::TRADER_NAME_1, GC::TRADER_NAME_1));
	mTraders.push_back(Trader(GC::TRADER_NAME_2, GC::TRADER_NAME_2, GC::TRADER_NAME_2));
	mTraders.push_back(Trader(GC::TRADER_NAME_3, GC::TRADER_NAME_3, GC::TRADER_NAME_3));
	mTraders.push_back(Trader(GC::TRADER_NAME_1, GC::TRADER_NAME_2, GC::TRADER_NAME_3));

	//sets up a line of traders
	struct SetupTraders
	{
		float x = -10.5f;
		const int biggestGap = 3;
		void operator()(Trader& t)
		{
			t.Initialize(GC::GO_TRADER);
			t.mpInstance->MaterialIndex = 0/*GameApp::Get().GetMaterialIndex("tileTex")*/;
			x+= 1.0f + (rand() % biggestGap);
			t.SetPos({ x, 0.0f, 4.0f });
		}
	};

	std::for_each(mTraders.begin(), mTraders.end(), SetupTraders()); // line of traders

	// Side doors
	mTraders.push_back(Trader(GC::TRADER_OBJ_1, GC::TRADER_OBJ_1, GC::TRADER_OBJ_1));
	mTraders.back().Initialize("CarBody");
	mTraders.back().SetPos({ 0.0f, 0.0f, 1.5f });
	mTraders.back().SetAsStoryItem();

	//Rear of car
	mTraders.push_back(Trader(GC::TRADER_OBJ_2, GC::TRADER_OBJ_2, GC::TRADER_OBJ_2));
	mTraders.back().Initialize("CarTire");
	mTraders.back().SetPos({ -1.0f, 0.0f, 1.5f });
	mTraders.back().SetAsStoryItem();

	//Front of car - engine
	mTraders.push_back(Trader(GC::TRADER_OBJ_3, GC::TRADER_OBJ_3, GC::TRADER_OBJ_3));
	mTraders.back().Initialize("CarTire");
	mTraders.back().SetPos({ 1.0f, 0.0f, 1.5f });
	mTraders.back().SetAsStoryItem();

}

bool PlayState::FindNearestTraderInRadius()
{
	struct SortByDistance
	{
		SimpleMath::Vector3 from;

		bool operator()(const Trader& a, const Trader& b)
		{
			distanceA = SimpleMath::Vector3::Distance( from , a.GetPosV3() );
			distanceB = SimpleMath::Vector3::Distance( from , b.GetPosV3() );

			return distanceA < distanceB;
		}
	private:
		float distanceA, distanceB;
	};

	SortByDistance s;
	// distance from player location
	s.from = mPlayer.GetPosV3();

	std::sort(mTraders.begin(), mTraders.end(),std::ref(s));

	float distance = SimpleMath::Vector3::Distance( mPlayer.GetPosV3() , mTraders.front().GetPosV3());

	if (distance < GC::TRADER_RADIUS)
	{
		mpActiveTrader = &mTraders.front();
		return true;
	}
	else
	{
		// if none in range clear any active trader
		mpActiveTrader = nullptr;
		return false;
	}


}

PlayState::PlayState()
	:
	mExperience(0, GC::EXP_EXPONENT, GC::EXP_OFFSET, 0)
{
}

void PlayState::Initialize()
{
	GameApp::Get().SetActiveCamera(&mCameras.at(CAMERA_TYPE::GAME));
	//GameApp::Get().SetActiveCamera(&mCamera);
	//mTile.Initialize("Tiles");
	mTileManager.Initialize();

	++mInventory["Radio"];
	++mInventory["Potion"];
	inventoryPosition = mInventory.begin();

	// todo change to closest trader in radius on button hit
	//mpActiveTrader = &mTempTrader;


	for (auto& c : mCameras)
	{
		c.SetPosition(0.0f, 25.0f, 0.0f);
		c.Pitch(XMConvertToRadians(CAM_ANGLE)); // SETS CAMERA TO FACE DOWN
	}

	mPlayer.Initialize("Player");
	mPlayerWeapon.Initialize("Weapon");

	mPlants.reserve(20);
	
	// ui bar
	mPlayerHealthBar.Initialise(GC::BAR_GRN, GC::BAR_RED);
	mPlayerHealthBar.SetMinMax(0, mPlayer.health); // todo change to max health
	mPlayerHealthBar.SetValue(mPlayer.health);
	mPlayerHealthBar.SetPosition({ 200.0f, 20.0f }); // todo add values to constants.h

	InitializeTraders();
	InitializeBuildings();
	

	//// tile clumping  HORIZONTAL FLOATING TILES CODE DEBUG REMOVE IF NEEDED
	//int n(0);
	//int clumpSize(5);
	//DirectX::XMFLOAT3 position = mTile.SetRandom();

	//while (n <= clumpSize)
	//{
	//	mTile.Initialize("Tiles");
	//	{
	//		mTile.mpInstance->MaterialIndex = 4;
	//		mTile.CreateTileWallHorizontal(clumpSize, position, n);
	//	}
	//	n++;
	//}

	// Setup temp enemies
	{
		// inserts n of enemies
		// TODO: (NOTE) ENEMIES ADDED HERE
		mEnemies.resize(3, Enemy(GC::ENEMY_TYPE_2, 25));

		//Init all enemies
		for (auto& e : mEnemies)
		{
			e.Initialize("Enemy");
			e.SetPosition({
						static_cast<float>(rand() % 10 + 2.0f),
						1.0f,
						static_cast<float>(rand() % 10 + 2.0f)
			});

			for(auto& t : mTraders)									//Check each trader in the game
				while (e.CheckCollision(e.GetPos(), t.GetPos()))	//Prevents enemies from spawning inside a trader
					e.SetPos({
						static_cast<float>(rand() % 10 + 2.0f),
						1.0f,
						static_cast<float>(rand() % 10 + 2.0f)
					});

		}

	}

	mCombatController.Initialize(&mPlayer,&mPlayerWeapon,&mEnemies);

	// Sprites
	{
		const DirectX::XMINT2 s = GameApp::Get().GetClientSizeU2(); //todo on resize
		SimpleMath::Vector2 v = GameApp::Get().GetClientSize();


		Sprite spriteSample;
		spriteSample.scale = 0.1f;

		spriteSample.Initialise("tileTex");
		mSprites["testSpriteFirst"] = spriteSample;

		spriteSample.Initialise("stoneTex");
		mSprites["testSpriteSecond"] = spriteSample;

		mScoreText.color = DirectX::Colors::White;
		mScoreText.fontIndex = 1;
		mScoreText.string = "Score: " + std::to_string(score);
		mScoreText.position = DirectX::SimpleMath::Vector2(650.f, 20.f);

		mScoreTextShadow.color = DirectX::Colors::Black;
		mScoreTextShadow.fontIndex = 1;
		mScoreTextShadow.string = "Score: " + std::to_string(score);
		mScoreTextShadow.position = DirectX::SimpleMath::Vector2(651.f, 21.f);


		// panel soruce
		const RECT src{ GC::PANEL_SRC[0],	GC::PANEL_SRC[1],	GC::PANEL_SRC[2],	GC::PANEL_SRC[3], };
		// inv panel
		{
			const RECT dst
			{
				s.x,							//  starts off viewport to the right
				10,								// padding
				s.x + (s.x / 3)+ 128,			//  third of viewport plus offset
				s.y - 10						// padding
			};

			mInventoryPanel.Initialize("uiTex", src, dst);

			mInventoryLocation.second.x = v.x - (v.x / 3);
			mInventoryLocation.second.y = 10;

			mInventoryLocation.first.x = v.x + 64;
			mInventoryLocation.first.y = 10;

			mInventoryText.color = DirectX::Colors::White;
			mInventoryText.fontIndex = 1;
			

		}

		//Help panel
		{
			const RECT dst
			{
				10,							//  starts off viewport to the right
				s.y,								// padding
				s.x - 10	,					// padding
				s.y + (s.y / 3) + 128			//  third of viewport plus offset
			};
			mHelpPanel.Initialize("uiTex", src, dst);


			mHelpLocation.second.y = v.y - (v.y / 3);
			mHelpLocation.second.x = 10;
			mHelpLocation.first.y = v.y + 64;
			mHelpLocation.first.x = 10;
			mHelpText.color = DirectX::Colors::White;
			mHelpText.fontIndex = 1;
			mHelpText.string = "Something helpful should go here"; //todo help text
		}

		// message text
		{
			mMessage.mText.center = true;
			mMessage.mText.string = "Start!";
			mMessage.mText.position = v / 2.0f;
			mMessage.mText.color = DirectX::Colors::Red;
		}
	}

	// needed in init for dirty frame
	for (auto& c : mCameras)
	{
		c.UpdateViewMatrix();
	}
	
}

void PlayState::reInitialize() { // USED TO LOAD A NEW MAP & ENEMIES, ETC, WHEN LEAVING AN AREA

	ReGen();
	// TODO: BELOW COULD CAUSE PROBLEMS & WILL DEFINITELY NEED REVISIONS
	// Setup temp enemies
	{
		// TODO: (NOTE) NEED TO ADD METHOD TO REMOVE CERTAIN ENEMIES & ADD NEW TYPES AT RANDOM RATHER THAN 
		//		REINITIALIZING THE SAME ENEMIES OVER AND OVER

		//Init all enemies
		for (auto& e : mEnemies)
		{
			e.mEnabled = true;
			e.SetPosition({
				static_cast<float>(rand() % 10 + 2.0f),
				1.0f,
				static_cast<float>(rand() % 10 + 2.0f)
			});

			for (auto& t : mTraders)								//Check each trader in the game
				while (e.CheckCollision(e.GetPos(), t.GetPos()))	//Prevents enemies from spawning inside a trader
					e.SetPos({
						static_cast<float>(rand() % 10 + 2.0f),
						1.0f,
						static_cast<float>(rand() % 10 + 2.0f)
					});
		}

	}

	mCombatController.Initialize(&mPlayer, &mPlayerWeapon, &mEnemies);

	mPlayer.AreaClear = false;
	mPlayer.genArea = false;
	mPlayer.SetPos({ 0.0f,0.0f,0.0f }); // could expand upon this to be fancy, load player at opposite side of grid

}


void PlayState::Update(const GameTimer & gt)
{
	//mTileManager.Update(gt);
	mCombatController.Update();

	//Transition to win state when collecting all items needed for the car
	if (TraderStoryComplete())
	{
		ResetState(gt);		//Resets in case you play again
		GameApp::Get().ChangeState("WinState");
	}

	for (auto& t : mTraders)		//Checks all traders in the game
	{
		if (mPlayer.CheckCollision(mPlayer.GetPositionWithVelocity(gt), t.GetPos()) ||		//Prevents player from walking through trader and being pushed by an enemy into trader
			mPlayer.CheckCollision(mPlayer.GetPos() + mPlayer.BouncebackPosition, t.GetPos()))
		{
			mPlayer.SetVelocity(0.0f);		//Prevents moving through the trader
			mPlayer.BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);		//Prevents the bounceback from happening
		}

		if (mPlayerWeapon.CheckCollision(mPlayerWeapon.GetPos(), t.GetPos()))	//Prevents weapon from going through trader
			mPlayerWeapon.ResetWeaponPosition();
	}


	mPlayer.Update(gt);

	// reset hazard timer for tile hazards 
	if (mPlayer.hazardTimer >= 0) {
		mPlayer.hazardTimer -= gt.DeltaTime();
	}

	// Game Camera follows player
	mCameras.at(CAMERA_TYPE::GAME).SetPosition(
		Lerp(mCameras.at(CAMERA_TYPE::GAME).GetPosition(), mPlayer.GetPos() + CAM_OFFSET, 0.9999f * gt.DeltaTime())
	);

	// MUST CHECK BOTH METHODS
	// currently -15 to 15 on both (BOUNDARIES SHOULD BE PUBLIC AND USED TO CALCULATE)
	// tiles are 32 by 32
	// 0.0f is middle of grid
	// each tile is 0.9375 of a world position
	// player position + half max tile * (MaxWorldPos / MaxTile) to find current tile (REPEAT FOR X & Y APPROPRIATELY)
	// LAST COMPONENT OF THE CALCULATION WILL CAUSE PROBLEMS IF SIZE OF EACH TILE ITSELF INCREASED
	//const float diff = float(mTileManager.MaxGen) / 30.0f;
	const float diff2 = 30.0f / float(mTileManager.MaxGen); // WORLDSPACE / GRID SIZE
	int underX = (int)round(mPlayer.GetPos().x); // worldspace position does not correspond to tilemap coordinate
	int underZ = (int)round(mPlayer.GetPos().z);
	float tileX = (underX + (0.5f * (mTileManager.MaxGen))/* * diff*/); // greater
	float tileZ = (underZ + (0.5f *(mTileManager.MaxGen)))/* * diff*/; // greater
	float tileX2 = (underX + (0.5f * (mTileManager.MaxGen)) * diff2); //lesser
	float tileZ2 = (underZ + (0.5f * (mTileManager.MaxGen)) * diff2); // lesser

	// USED FOR DEBUGGING THE SAFETY NET FOR THE PLAYER GRID CHECKER - KEEP IF ERRORS ARISE LATER
	//if ((tileX < 0 || tileZ < 0 || tileX > mTileManager.MaxGen || tileZ > mTileManager.MaxGen) ||
	//	(tileX2 < 0 || tileZ2 < 0 || tileX2 > mTileManager.MaxGen || tileZ2 > mTileManager.MaxGen)) {
	//
	//	int r = 0;
	//}

	// if player over the grid execute the grid check 
	if ((tileX > 0 && tileZ > 0 && tileX < mTileManager.MaxGen && tileZ < mTileManager.MaxGen) && 
		(tileX2 >= 0 && tileZ2 >= 0 && tileX2 <= mTileManager.MaxGen && tileZ2 <= mTileManager.MaxGen)) {
		
		// POSSIBLE GLITCH HERE (TILE ACTIVE WHEN PLAYER NOT OVER IT, TILE RIGHT / RIGHT&UP) - FIXED ITSELF??
		// 
		// if the player is over a poison/damage tile
		if (mTileManager.GetIndex(static_cast<int>(tileX), static_cast<int>(tileZ)) == mTileManager.Haz1Tex || 
			mTileManager.GetIndex(static_cast<int>(tileX2), static_cast<int>(tileZ2)) == mTileManager.Haz1Tex) {
			if (mPlayer.hazardTimer <= 0) { // if hazard should be active
				mPlayer.health -= 5;
				mPlayerHealthBar.SetValue(mPlayer.health);
				mPlayer.hazardTimer = 3; // reset hazard timer 

			}
		}

		// if the player is over a slow tile
		if (mTileManager.GetIndex(static_cast<int>(tileX), static_cast<int>(tileZ)) == mTileManager.Haz2Tex ||
			mTileManager.GetIndex(static_cast<int>(tileX2), static_cast<int>(tileZ2)) == mTileManager.Haz2Tex) {
			mPlayer.Slowed = true;
		}

		if (mTileManager.GetIndex(static_cast<int>(tileX), static_cast<int>(tileZ)) != mTileManager.Haz2Tex &&
			mTileManager.GetIndex(static_cast<int>(tileX2), static_cast<int>(tileZ2)) != mTileManager.Haz2Tex) {
			mPlayer.Slowed = false;
		}
	
		if (mTileManager.GetIndex(static_cast<int>(tileX), static_cast<int>(tileZ)) == mTileManager.Haz3Tex ||
			mTileManager.GetIndex(static_cast<int>(tileX2), static_cast<int>(tileZ2)) == mTileManager.Haz3Tex) {
			mPlayer.Slippy = true;
		}

		if (mTileManager.GetIndex(static_cast<int>(tileX), static_cast<int>(tileZ)) != mTileManager.Haz3Tex &&
			mTileManager.GetIndex(static_cast<int>(tileX2), static_cast<int>(tileZ2)) != mTileManager.Haz3Tex) {
			mPlayer.Slippy = false;
		}
	}
	

	int i = 0;
	std::for_each(mEnemies.begin(), mEnemies.end(), [&](Enemy& e)
	{ 
		if (e.mEnabled)
		{
			//Enemy look at players position (only do when in range), only look when not attacking either
			XMVECTOR playerPosition = XMLoadFloat3(&mPlayer.GetPos());

			if (e.GetType() == GC::ENEMY_TYPE_1) { // ENEMY TYPE EXCLUSIVE LOGIC LOCATED HERE
				// TODO: (NOTE) IF PLAYER IN RANGE OF SIGHT LOCATED HERE, COULD IMPROVE & IMPLEMENT FOR OTHER ENEMY TYPES
				if (mPlayer.GetPos().x >= (e.GetPos().x - GC::ENEMYTYPE1_RANGE) &&
					mPlayer.GetPos().x <= (e.GetPos().x + GC::ENEMYTYPE1_RANGE)) { // player within - range on x
					if (mPlayer.GetPos().z >= (e.GetPos().z - GC::ENEMYTYPE1_RANGE) &&
						mPlayer.GetPos().z <= (e.GetPos().z + GC::ENEMYTYPE1_RANGE)) { // player within - range on z
						e.LookAt(playerPosition);
					}
				}

				// enemy movement behaviour based on player radius
				if (DirectX::SimpleMath::Vector3::Distance(mPlayer.GetPos(), e.GetPos()) < GC::ENEMYTYPE1_RANGE &&
					e.getattacking().isAttacking == false)
				{
					e.mBehaviour = Enemy::Behaviour::CHASE;
				}
				else
				{
					e.mBehaviour = Enemy::Behaviour::NONE;
				}
			}

			if (e.GetType() == GC::ENEMY_TYPE_2) { // ENEMY TYPE EXCLUSIVE LOGIC LOCATED HERE
				// TODO: (NOTE) IF PLAYER IN RANGE OF SIGHT LOCATED HERE, COULD IMPROVE & IMPLEMENT FOR OTHER ENEMY TYPES
				if (mPlayer.GetPos().x >= (e.GetPos().x - GC::ENEMYTYPE2_RANGE) &&
					mPlayer.GetPos().x <= (e.GetPos().x + GC::ENEMYTYPE2_RANGE)) { // player within - range on x
					if (mPlayer.GetPos().z >= (e.GetPos().z - GC::ENEMYTYPE2_RANGE) &&
						mPlayer.GetPos().z <= (e.GetPos().z + GC::ENEMYTYPE2_RANGE)) { // player within - range on z
						e.LookAt(playerPosition);
					}
				}

				// enemy movement behaviour based on player radius
				if (DirectX::SimpleMath::Vector3::Distance(mPlayer.GetPos(), e.GetPos()) < GC::ENEMYTYPE2_RANGE &&
					e.getattacking().isAttacking == false)
				{
					e.mBehaviour = Enemy::Behaviour::CHASE;
				}
				else
				{
					e.mBehaviour = Enemy::Behaviour::NONE;
				}
			}

			// enemy collision with planyer
			if (mPlayer.CheckCollision(mPlayer.GetPos(), e.GetPos()))
			{
				mPlayer.DamagePlayer(e.GetAttack());
				mPlayerHealthBar.SetValue(mPlayer.health);
				GameApp::Get().GetAudio().Play("playerHit01", nullptr, false, 1.0f, GetRandomVoicePitch());
				//Transition to game over state
				if (mPlayer.health <= 0)
				{
					ResetState(gt);
					GameApp::Get().ChangeState("GameOver");
				}
			}

			//When checking if enemy is in range, have this be in that section to prevent enemy particles from far from being checked
			for (auto& p : e.particles)
			{
				if (mPlayer.CheckCollision(mPlayer.GetPos(), p.GetPos()))
				{
					mPlayer.DamagePlayer(e.GetAttack());
					mPlayerHealthBar.SetValue(mPlayer.health);
					GameApp::Get().GetAudio().Play("playerHit01", nullptr, false, 1.0f, GetRandomVoicePitch());
					//Transition to game over state
					if (mPlayer.health <= 0)
					{
						ResetState(gt);
						GameApp::Get().ChangeState("GameOver");
					}
					break;
				}
			}

			if (mPlayerWeapon.CheckCollision(mPlayerWeapon.GetPos(), e.GetPos()))
			{
				e.DamageEnemy(mPlayer.attack);		//Takes away health from enemy + blowsback enemy position
				if (e.GetHealth() <= 0)
				{
					// gain exp
					mExperience.AddExp(GC::EXP_DEFAULT);
					score += GC::SCORE_ENEMY;

					//Could be put into an exists function in Inventory Class
					std::string droppedItem = e.GetDropItem();
					if (droppedItem != "Empty")		//I.e An item was dropped from enemy
					{
						++mInventory[droppedItem];
					}

					e.MoveOffScreen();
					e.mEnabled = false;

					GameApp::Get().GetAudio().Play("EnemyDie1", nullptr, false, 1.0f, GetRandomVoicePitch());
				}
				else
				{
					GameApp::Get().GetAudio().Play("EnemyHit1", nullptr, false, 1.0f, GetRandomVoicePitch());
				}
			}

			// enemy collides with traders
			for (auto& t : mTraders)
				if (e.CheckCollision(e.GetPos() + e.BouncebackPosition, t.GetPos()))		//If there is a collision between any of the traders and the bounceback position of the enemy
					e.BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

			e.SetVelocity(mPlayer.GetPos(), gt);

		}

		e.Update(gt);

		{
			SimpleMath::Vector3 pos = e.GetPos();

			GameApp::Get().mDebugLog << "Enemy i:" << i
				<< "  Health: " << e.GetHealth()
				<< "  mpInstance: " << e.mpInstance
				<< "  X: " << std::setprecision(2) << pos.x
				<< "  Z:" << std::setprecision(2) << pos.z << "\n";
		}
		

		i++;

	});

	PassConstants* pMainPassCB = GameApp::Get().GetMainPassCB();

	//flashing red for low health
	if (mPlayer.health <= GC::PLAYER_LOW_HEALTH)
	{
		float strength = sin(gt.TotalTime()*5.0f) * 0.5f + 0.5f;

		pMainPassCB->Lights[1].Strength = { strength ,0.1f,0.1f };

		// Less intense vibration
		strength *= 0.1f;

		Input::Get().SetVibration(strength, strength);
	}
	else
	{
		pMainPassCB->Lights[1].Strength = { 0.0f, 0.0f, 0.0f };
	}

	UpdatePlants(gt);



	UiUpdate(gt);

	// Sprite update
	


	if (mExperience.HasLeveledUp())
	{
		mMessage.Activate("LEVEL UP", 5.0f);
		score += GC::SCORE_LVLUP;
		//GameApp::Get().ChangeState("PauseMenu");
	}

	// show/hide item menu
	if (itemMenuOpen)
	{
		mInventoryText.string = "Inventory:  (size " + std::to_string( mInventory.size()) + ")\n";
		//GameApp::Get().mDebugLog << "Inventory:  (size " << mInventory.size() << ")\n";

		
		std::for_each(mInventory.begin(), mInventory.end(), [&](auto& inv)
		{
			if (inventoryPosition != mInventory.end())
			{
				//Debug purposes: shows the currently selected item based on inventoryPosition value
				GameApp::Get().mDebugLog << "Current Selected Item: " << (*inventoryPosition).first << " x" << (*inventoryPosition).second << "\n";

				if((*inventoryPosition).first == inv.first)
				{
					mInventoryText.string += ">";
				}
				else
				{
					mInventoryText.string += " ";
				}

			}
			mInventoryText.string += " " + inv.first + " (" + std::to_string(inv.second) + ")\n";

		});
	}

	// IMPLEMENT CHECK FOR ENEMIES HERE
	if (EnemiesRemaining() == 0 && !mPlayer.AreaClear)
		mPlayer.AreaClear = true;

	if (mPlayer.AreaClear && mPlayer.genArea) { // TODO: IMPLEMENT CHANGE STATE FOR NEW AREA HERE
		// change state, trigger regen
		reInitialize(); // MAY CAUSE ERROR IF USED HERE
		GameApp::Get().ChangeState("NewArea1");
	}

	// for dirty frame
	for (auto& c : mCameras)
	{
		c.UpdateViewMatrix();
	}





	/*mEnemies.erase(std::remove_if(mEnemies.begin(), mEnemies.end(), [](Enemy& e)
	{
		return e.GetHealth() <= 0;

	}), mEnemies.end());*/

}

void PlayState::Draw(const GameTimer & gt)
{
	// UI

	std::for_each(mSprites.begin(), mSprites.end(), [](auto& sp)
	{
		sp.second.Draw();
	});

	mPlayerHealthBar.Draw();

	mInventoryPanel.Draw();
	mInventoryText.Draw();

	mHelpPanel.Draw();
	mHelpText.Draw();

	mMessage.Draw();

	mScoreTextShadow.Draw();
	mScoreText.Draw();

}

void PlayState::OnMouseDown(WPARAM btnState, int x, int y)
{
}

void PlayState::OnMouseUp(WPARAM btnState, int x, int y)
{
	//if (((btnState & MK_LBUTTON) != 0))
	//{

	//	POINT lastMousePos = GameApp::Get().GetLastMousePos();

	//	// Make each pixel correspond to a quarter of a degree.
	//	float dx = XMConvertToRadians(0.25f*static_cast<float>(x - lastMousePos.x));
	//	float dy = XMConvertToRadians(0.25f*static_cast<float>(y - lastMousePos.y));

	//	mCameras.at(CAMERA_TYPE::GAME).Pitch(dy);
	//	mCameras.at(CAMERA_TYPE::GAME).RotateY(dx);
	//}
}

void PlayState::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void PlayState::OnKeyboardInput(const GameTimer & gt)
{

	Controls(gt); 

}

void PlayState::OnResume()
{
	// remove focus from trader
	mpActiveTrader = nullptr;
}

/*

If gamepad
	gamepad
else keyboard
	keyboard
	switch cam
		play then keyboard player move 
		debug then keyboard debug cam move
*/

void PlayState::Controls(const GameTimer & gt)
{


	//bool playerMoved = false;

	//Gamepad
	if (Input::Get().GamePadConnected())
	{

		Gamepad(gt);

	}
	else // Keyboard controls. Game/Debug cam
	{
		switch (mCamType)
		{
		case CAMERA_TYPE::GAME:
		{
			Keyboard(gt);
		}
		break;
		case CAMERA_TYPE::DEBUG:
		{
			KeyboardDebug(gt);
		}
		break;
		}

		
	}

	
}

void PlayState::InventoryUp()
{
	if (inventoryPosition == mInventory.begin())
	{
		inventoryPosition = mInventory.rbegin().base(); // last element 
	}

	--inventoryPosition;
}

void PlayState::InventoryDown()
{
	++inventoryPosition;

	if (inventoryPosition == mInventory.end())
	{
		inventoryPosition = mInventory.begin();						//Loop back round to top of inventory
	}
}

void PlayState::ItemAction()
{
	assert(inventoryPosition != mInventory.end());

	switch (GC::ITEM_LIST.at((*inventoryPosition).first).category)
	{
	case  ItemCategory::Healing:
		if (inventoryPosition->second > 0)
		{
			--inventoryPosition->second; //removes a potion
			mPlayer.health += GC::HEAL_SMALL;
			if (mPlayer.health > 100)	mPlayer.health = 100;	//Replace 100 with a constant max player variable
			mPlayerHealthBar.SetValue(mPlayer.health);
		}
		break;
	case ItemCategory::Weapons:
		if (mCombatController.GetCurrentWeapon() != (*inventoryPosition).first)
		{
			mCombatController.EquipWeapon((*inventoryPosition).first);
		}
		break;
	case ItemCategory::Farming:
		// if there isnt a plant in a radius then instance 
		if (CreatePlant())
		{
			--inventoryPosition->second;
		}

		break;
	case ItemCategory::KeyItems:


		break;
	}

	CleanInventory(mInventory); // tidy up

}

void PlayState::CleanInventory(Inventory& inv)
{
	for (Inventory::iterator it = inv.begin(); it != inv.end();)
	{
		if ((*it).second <= 0) 
		{
			it = inv.erase(it);
			inventoryPosition = inv.begin();
		}
		else {
			++it;
		}
	}
}

void PlayState::ReGen() { // DOESN'T WORK PROPERLY, JUST CUTS FRAME RATE, NEED TO REMOVE TEXTURES FROM OBJECTS
	
	mTileManager.REGEN();
}

void PlayState::UiUpdate(const GameTimer & gt)
{
	mSprites["testSpriteFirst"].rotation = cosf(gt.TotalTime()) * 0.1f;
	mSprites["testSpriteSecond"].rotation = sinf(gt.TotalTime());

	mPlayerHealthBar.Update(gt);

	// animate inventory toggle
	SimpleMath::Vector2 move = SimpleMath::Vector2::Lerp(mInventoryPanel.GetPos(), mInventoryLocation.first,gt.DeltaTime() * 2.0f); // todo Luc move to constant.h
	mInventoryPanel.Move(move, false);
	// padding for text
	move += SimpleMath::Vector2{32.0f, 32.0f}; //todo luc move to constants.h
	mInventoryText.position = move;

	//animate help toggle
	move = SimpleMath::Vector2::Lerp(mHelpPanel.GetPos(), mHelpLocation.first, gt.DeltaTime() * 2.0f); // todo Luc
	mHelpPanel.Move(move, false);
	//padding for help text
	move += SimpleMath::Vector2{ 32.0f, 32.0f };//todo luc move to constants.h
	mHelpText.position = move;

	mScoreText.string = "Score: " + std::to_string(score);
	mScoreTextShadow.string = mScoreText.string;

	mMessage.Update(gt);
}

bool PlayState::CreatePlant()
{
	
	// to tile nearest to player
	SimpleMath::Vector3 pos(mPlayer.GetPosV3());
	pos.x = round(pos.x);
	pos.y = round(pos.y);
	pos.z = round(pos.z);

	// check if plant is already at location
	bool found = false;
	int i = 0;

	while (i < mPlants.size() && !found)
	{
		if (SimpleMath::Vector3::Distance(pos, mPlants.at(i).GetPosV3()) < 0.1f) 
		{
			// There is already a plant at this location
			found = true;
		}
		++i;
	}


	if (!found)
	{
		// todo Luc change basic on item rather than random
		int r = rand() % 3;
		switch (r)
		{
		case 0:	mPlants.push_back(Plant(GC::PLANT_0)); break;
		case 1:	mPlants.push_back(Plant(GC::PLANT_1)); break;
		case 2:	mPlants.push_back(Plant(GC::PLANT_2)); break;
		default:
			break;
		}

		mPlants.back().Initialize("Potato");
		mPlants.back().SetPos(pos);
	}

	return !found;
	
}

void PlayState::UpdatePlayer(const GameTimer & gt)
{
}

void PlayState::UpdatePlants(const GameTimer & gt)
{
	for (auto& p : mPlants)
	{
		p.Update(gt);
	}
}

void PlayState::UpdateTiles(const GameTimer & gt)
{
}

void PlayState::UpdateEnemies(const GameTimer & gt)
{
}

void PlayState::HarvestByRadius()
{
	for (auto& p : mPlants)
	{
		// player within range
		if (SimpleMath::Vector3::Distance(mPlayer.GetPosV3(), p.GetPosV3()) < 1.5f) // todo Luc move to constant.h
		{
			if (p.Harvest() ) // applies changes to instance state
			{
				// add all items from lookup to inventory
				for (auto& m : GC::PLANT_LOOKUP_HARVEST.at( p.GetName() ) )
				{
					mInventory[m.first] += m.second;
				}

				score += GC::SCORE_PLANT;
			}
		}
	}
}

void PlayState::Keyboard(const GameTimer & gt)
{

	//Player keyboard movement
	if (Input::Get().KeyHeld(GC::KEY_FW))
	{
		mPlayer.MoveUp(gt);
	}

	if (Input::Get().KeyHeld(GC::KEY_BK))
	{
		mPlayer.MoveDown(gt);
	}

	if (Input::Get().KeyHeld(GC::KEY_RT))
	{
		mPlayer.MoveRight(gt);
	}

	if (Input::Get().KeyHeld(GC::KEY_LT))
	{
		mPlayer.MoveLeft(gt);
	}

	//Player attack
	if (Input::Get().KeyReleased(GC::KEY_ATTACK))
	{
		mCombatController.PlayerAttack();
	}

	//Switch to debug cam
	if (Input::Get().KeyReleased(GC::KEY_CAM))
	{
		mCamType = CAMERA_TYPE::DEBUG;

		//Set debug cam to game position to avoid getting lost
		mCameras.at(CAMERA_TYPE::DEBUG).SetPosition(mCameras.at(CAMERA_TYPE::GAME).GetPosition3f());

		//Update active camera
		GameApp::Get().SetActiveCamera(&mCameras.at(CAMERA_TYPE::DEBUG));
	}

	// Plant

	// Debug Planting
	if (Input::Get().KeyReleased('7'))
	{
		CreatePlant();
	}

	//Harvest
	if (Input::Get().KeyReleased('8'))
	{
		HarvestByRadius();
	}

	// Inventory

	// Scrolling through inventory keys Debug
	if (Input::Get().KeyReleased(VK_DOWN) & itemMenuOpen)
	{
		InventoryDown();
	}

	if (Input::Get().KeyReleased(VK_UP) & itemMenuOpen)
	{
		InventoryUp();
	}

	// Opens and closes item menu
	if (Input::Get().KeyReleased(GC::KEY_INVENTORY))
	{
		itemMenuOpen = !itemMenuOpen;
		std::swap(mInventoryLocation.first, mInventoryLocation.second);
	}

	// Use Item Key, change to something else, or only allow when on item/pause menu
	if (Input::Get().KeyReleased(GC::KEY_USEITEM) & itemMenuOpen)
	{
		ItemAction();
	}

	// Toggle Help
	if (Input::Get().KeyReleased('H'))
	{
		std::swap(mHelpLocation.first, mHelpLocation.second);
	}

	// Trade
	if (Input::Get().KeyReleased('9'))
	{
		if (FindNearestTraderInRadius())
		{
			//todo play sound
			assert(mpActiveTrader);
			GameApp::Get().ChangeState(GC::STATE_TRADE);
		}
		else
		{
			//todo sound fail sound
		}
	}
	
	// Debug generate
	if (Input::Get().KeyReleased(GC::KEY_DEBUG_GENERATE)) //todo VOID NEEDS FIXING FIRST
	{
		ReGen();
	}

	if (Input::Get().KeyReleased('M'))
	{
		//mEnemies.clear();

		for (auto& e : mEnemies)
		{
			e.mEnabled = false;
			e.MoveOffScreen();
		}
	}
}

void PlayState::KeyboardDebug(const GameTimer & gt)
{

	const float dt = gt.DeltaTime();

	//dpad debug cam
	if (Input::Get().KeyHeld(GC::KEY_FW))
	{
		mCameras.at(CAMERA_TYPE::DEBUG).Walk(GC::ZOOM_SPEED * dt);
	}

	if (Input::Get().KeyHeld(GC::KEY_BK))
	{
		mCameras.at(CAMERA_TYPE::DEBUG).Walk(-GC::ZOOM_SPEED * dt);
	}

	if (Input::Get().KeyHeld(GC::KEY_RT))
	{
		mCameras.at(CAMERA_TYPE::DEBUG).Strafe(GC::MOVE_SPEED * dt);
	}

	if (Input::Get().KeyHeld(GC::KEY_LT))
	{
		mCameras.at(CAMERA_TYPE::DEBUG).Strafe(-GC::MOVE_SPEED * dt);
	}

	if (Input::Get().KeyHeld(GC::KEY_RAISE))
	{
		mCameras.at(CAMERA_TYPE::DEBUG).Elevate(GC::MOVE_SPEED * dt);
	}

	if (Input::Get().KeyHeld(GC::KEY_LOWER))
	{
		mCameras.at(CAMERA_TYPE::DEBUG).Elevate(-GC::MOVE_SPEED * dt);
	}

	// Switch to game camera
	if (Input::Get().KeyReleased(GC::KEY_CAM))
	{
		mCamType = CAMERA_TYPE::GAME;
		//Update active camera
		GameApp::Get().SetActiveCamera(&mCameras.at(CAMERA_TYPE::GAME));
	}

	// Debug random enemy pos
	if (Input::Get().KeyReleased(GC::KEY_DEBUG_ENEMY_POS))
	{
		std::for_each(mEnemies.begin(), mEnemies.end(), [](Enemy& e)
		{
			e.mEnabled = true;
			e.SetRandomPosition();
		});
	}
}

void PlayState::Gamepad(const GameTimer & gt)
{

	// Left stick
	mPlayer.Move(gt, Input::Get().LeftStickXZ()* GC::MOVE_SPEED);

	// Attack
	if (Input::Get().GamePad().rightTrigger == ButtonState::HELD)
	{
		mCombatController.PlayerAttack();
	}

	if (Input::Get().GamePad().start == ButtonState::PRESSED)
	{
		GameApp::Get().ChangeState("PauseMenu");
	}

	if (Input::Get().GamePad().y == ButtonState::PRESSED)
	{
		itemMenuOpen = !itemMenuOpen;
	}

	if (Input::Get().GamePad().dpadUp == ButtonState::PRESSED)
	{
		InventoryUp();
	}

	if (Input::Get().GamePad().dpadDown == ButtonState::PRESSED)
	{
		InventoryDown();
	}

	if (Input::Get().GamePad().a == ButtonState::PRESSED)
	{
		ItemAction();
	}

}

bool PlayState::TraderStoryComplete()
{
	int count = 0;

	for (auto& t : mTraders)
	{
		if (t.CompletedStoryItem()) 
		{
			++count;
		}
	}

	return count >= 3; // todo constant to header
}

void PlayState::ResetState(const GameTimer& gt)
{
	score = 0;		//Reset Score

	//Reset Camera
	for (auto& c : mCameras)
	{
		c.SetPosition(0.0f, 25.0f, 0.0f);
		c.UpdateViewMatrix();
	}

	mPlayer.Reset(gt);
	mCombatController.Reset();
	mPlayerWeapon.Reset(gt);
	mPlayerHealthBar.SetValue(100);

	//Reset Inventory
	mInventory.clear();
	++mInventory["Radio"];
	++mInventory["Potion"];
	inventoryPosition = mInventory.begin();

	//Reset Traders
	float x = -10.5f;
	const int biggestGap = 3;
	for (auto& t : mTraders)
	{
		if (!t.GetIfStoryItem())		//Keeps position of car the same
		{
			x += 1.0f + (rand() % biggestGap);
			t.SetPos({ x, 0.0f, 3.0f });
		}

		t.ResetTrader();		//Resets quests/trades for each trader
	}

	//Reset Plants
	for (auto& p : mPlants)
	{
		p.SetPos(DirectX::XMFLOAT3(0.0f, -200.f, 0.0f));
	}
	mPlants.clear();

	//Reset Tiles
	ReGen();

	//Reset Enemies
	for (auto& e : mEnemies)
	{
		e.mEnabled = true;
		e.mpInstance->MaterialIndex = 0;
		e.Reset();
		e.SetPos({
		static_cast<float>(rand() % 10 + 2.0f),
			1.0f,
		static_cast<float>(rand() % 10 + 2.0f)
			});

		for (auto t : mTraders)
			while (e.CheckCollision(e.GetPos(), t.GetPos()))	//Prevents enemies from spawning inside a trader
				e.SetPos({
					static_cast<float>(rand() % 10 + 2.0f),
					1.0f,
					static_cast<float>(rand() % 10 + 2.0f)
					});
	}
}
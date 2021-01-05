#include "XmfloatOverload.h"
#include "PlayState.h"
#include "GameApp.h"
#include "XmfloatOverload.h"
#include "SimpleMath.h"
//#include "Input.h"
#include "Constants.h"
#include <unordered_map>
#include <algorithm>

using ButtonState = GamePad::ButtonStateTracker::ButtonState;

void PlayState::InitializeTraders()
{
	
	mTraders.push_back(Trader(GC::TRADER_NAME_TEST, GC::TRADER_NAME_TEST, GC::TRADER_NAME_TEST));
	mTraders.push_back(Trader(GC::TRADER_NAME_1, GC::TRADER_NAME_1, GC::TRADER_NAME_1));
	mTraders.push_back(Trader(GC::TRADER_NAME_2, GC::TRADER_NAME_2, GC::TRADER_NAME_2));
	mTraders.push_back(Trader(GC::TRADER_NAME_3, GC::TRADER_NAME_3, GC::TRADER_NAME_3));
	mTraders.push_back(Trader(GC::TRADER_NAME_1, GC::TRADER_NAME_2, GC::TRADER_NAME_3));

	struct SetupTraders
	{
		float x = -10.5f;
		const int biggestGap = 3;
		void operator()(Trader& t)
		{
			t.Initialize(GC::GO_TRADER);
			t.mpInstance->MaterialIndex = GameApp::Get().GetMaterialIndex("tileTex");
			x+= 1.0f + (rand() % biggestGap);
			t.SetPos({ x, 0.0f, 3.0f });
		}
	};

	std::for_each(mTraders.begin(), mTraders.end(), SetupTraders());

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
	
	/*mTile.Initialize("Tiles");
	{
		mTile.mpInstance->MaterialIndex = 4;*/
		/*mTile.SetRandomPosition();
	}*/

	InitializeTraders();
	
	int n(0);
	int clumpSize(5);
	DirectX::XMFLOAT3 position = mTile.SetRandom();

	while (n <= clumpSize)
	{
		mTile.Initialize("Tiles");
		{
			mTile.mpInstance->MaterialIndex = 4;
			mTile.CreateTileWallHorizontal(clumpSize, position, n);
		}
		n++;
	}


	// Setup temp enemies
	{
		// inserts n of enemies
		mEnemies.push_back(Enemy("EnemyType1", 25));
		mEnemies.push_back(Enemy("EnemyType1", 25));
		mEnemies.push_back(Enemy("EnemyType1", 25));

		//Init all enemies
		std::for_each(mEnemies.begin(), mEnemies.end(), [](Enemy& e) 
		{ 
			e.Initialize("Enemy"); 

			e.SetPosition({
				static_cast<float>(rand() % 10 + 2.0f),
				1.0f,
				static_cast<float>(rand() % 10 + 2.0f)
			});
		});
	}

	mCombatController.Initialize(&mPlayer,&mPlayerWeapon,&mEnemies);

	

	
	// Sprites
	{
		Sprite spriteSample;
		spriteSample.scale = 0.1f;

		spriteSample.Initialise("tileTex");
		mSprites["testSpriteFirst"] = spriteSample;

		spriteSample.Initialise("stoneTex");
		mSprites["testSpriteSecond"] = spriteSample;
	}


	// needed in init for dirty frame
	for (auto& c : mCameras)
	{
		c.UpdateViewMatrix();
	}
	
}

void PlayState::Update(const GameTimer & gt)
{
	//mTileManager.Update(gt);
	mPlayer.Update(gt);
	mCombatController.Update();

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
		e.Update(gt); 
		if (mCombatController.CheckCollision(mPlayer.GetPos(), e.GetCollisionPoint()))
		{
			mPlayer.DamagePlayer(e.GetAttack());
			GameApp::Get().GetAudio().Play("playerHit01", nullptr, false, 1.0f,GetRandomVoicePitch());
		}

		if (mCombatController.CheckCollision(
			e.mpInstance->World._41,
			e.mpInstance->World._42,
			e.mpInstance->World._43 ))
		{
			e.DamageEnemy(mPlayer.attack);		//Takes away health from enemy + blowsback enemy position
			if (e.GetHealth() < 0)
			{
				// gain exp
				mExperience.AddExp(GC::EXP_DEFAULT); 

				//Could be put into an exists function in Inventory Class
				std::string droppedItem = e.GetDropItem();
				if (droppedItem != "Empty")		//I.e An item was dropped from enemy
				{
					++mInventory[droppedItem];
				}

				e.mpInstance->World._42 -= 200.0f;
				//e.mpInstance = nullptr;
				mEnemies.erase(mEnemies.begin() + i);

				GameApp::Get().GetAudio().Play("EnemyDie1", nullptr, false, 1.0f, GetRandomVoicePitch());
			}
			else
			{
				GameApp::Get().GetAudio().Play("EnemyHit1", nullptr, false, 1.0f, GetRandomVoicePitch());
			}
		}
		i++;
	
	});

	PassConstants* pMainPassCB = GameApp::Get().GetMainPassCB();

	//flashing red for low health
	if (mPlayer.health <= GC::PLAYER_LOW_HEALTH)
	{
		float strength = sin(gt.TotalTime()*5.0f) * 0.5f + 0.5f;

		pMainPassCB->Lights[0].Strength = { strength ,0.0f,0.0f };

		// Less intense vibration
		strength *= 0.1f;

		Input::Get().SetVibration(strength, strength);

	}

	else
	{
		pMainPassCB->Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
	}

	//mCamera.SetPosition(0, 50, 0);

	// Sprite update
	mSprites["testSpriteFirst"].rotation = cosf(gt.TotalTime()) * 0.1f;
	mSprites["testSpriteSecond"].rotation = sinf(gt.TotalTime());


	if (mExperience.HasLeveledUp())
	{
		GameApp::Get().ChangeState("PauseMenu");
	}


	// show/hide item menu
	if (itemMenuOpen)
	{
		GameApp::Get().mDebugLog << "Inventory:  (size " << mInventory.size() << ")\n";

		//Debug purposes: shows which items are in inventory
		std::for_each(mInventory.begin(), mInventory.end(), [&](auto& inv)
		{
			GameApp::Get().mDebugLog << inv.first << " : " << inv.second << "\n";
		});


		//Debug purposes: shows the currently selected item based on inventoryPosition value
		if (inventoryPosition != mInventory.end())
			GameApp::Get().mDebugLog << "Current Selected Item: " << (*inventoryPosition).first << " x" << (*inventoryPosition).second << "\n";
	}

	//If players health is below 0, change state to Game Over state
	if (mPlayer.health <= 0)
	{
		mPlayer.health = 100;		//Just to check that restart changes state, will load most recent save instead
		GameApp::Get().ChangeState("GameOver");
	}



	// for dirty frame
	for (auto& c : mCameras)
	{
		c.UpdateViewMatrix();
	}
}

void PlayState::Draw(const GameTimer & gt)
{

	std::for_each(mSprites.begin(), mSprites.end(), [](auto& sp)
	{
		sp.second.Draw();
	});

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

	const float dt = gt.DeltaTime();

	//bool playerMoved = false;

	//Gamepad
	if (Input::Get().GamePadConnected())
	{

		// Left stick
		mPlayer.Move(  gt, Input::Get().LeftStickXZ()* GC::MOVE_SPEED);

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
	else // Keyboard controls. Game/Debug cam
	{
		switch (mCamType)
		{
		case CAMERA_TYPE::GAME:
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

			if (Input::Get().KeyReleased(GC::KEY_DEBUG_GENERATE)) // VOID NEEDS FIXING FIRST
			{
				ReGen();
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
		}
		break;
		case CAMERA_TYPE::DEBUG:
		{
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
				mEnemies.push_back(Enemy("EnemyType1", 25));
				mEnemies.back().Initialize("Enemy");

				std::for_each(mEnemies.begin(), mEnemies.end(), [](Enemy& e)
				{
					e.SetRandomPosition();
				});
			}
		}
		break;
		}

		//Scrolling through inventory keys Debug
		if (Input::Get().KeyReleased(VK_DOWN) & itemMenuOpen)
		{
			InventoryDown();
		}

		if (Input::Get().KeyReleased(VK_UP) & itemMenuOpen)
		{
			InventoryUp();
		}

		//Opens and closes item menu
		if (Input::Get().KeyReleased(GC::KEY_INVENTORY))
		{
			itemMenuOpen = !itemMenuOpen;
		}
	}

	//Use Item Key, change to something else, or only allow when on item/pause menu
	if (Input::Get().KeyReleased(GC::KEY_USEITEM) & itemMenuOpen)
	{
		ItemAction();
	}

	// trade
	//todo move to trade state
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
		}
		break;
	case ItemCategory::Weapons:
		if (mCombatController.GetCurrentWeapon() != (*inventoryPosition).first)
		{
			mCombatController.EquipWeapon((*inventoryPosition).first);
		}
		break;
	case ItemCategory::Farming:
		--inventoryPosition->second;
		// if there isnt a plant in a radius then instance 
		//todo create an instance of a 'plant' gameobject at players location

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
	srand(static_cast<int>(time(NULL)));
	std::mt19937 rng(static_cast<int>(time(NULL))); // seed the generator

	std::uniform_int_distribution<int> grid(0, mTileManager.mDimention);
	std::uniform_int_distribution<int> gen1(mTileManager.H1MinSize, mTileManager.H1MaxSize); // uniform, unbiased
	std::uniform_int_distribution<int> gen2(mTileManager.H2MinSize, mTileManager.H2MaxSize); // uniform, unbiased
	std::uniform_int_distribution<int> gen3(mTileManager.H3MinSize, mTileManager.H3MaxSize); // uniform, unbiased

	std::vector<std::vector<int>> coords(mTileManager.mDimention);
	for (int u = 0; u < mTileManager.mDimention; u++) {
		coords[u].resize(mTileManager.mDimention);
	} // setup coords array to grid size

	std::vector<std::vector<int>> origin(mTileManager.mDimention); // used to store central tiles only during the first spawn
	for (int u = 0; u < mTileManager.mDimention; u++) {
		origin[u].resize(mTileManager.mDimention);
	} // setup coords array to grid size

	// select positions for central hazard spots at random and store in vector
	// use distance here as well

	// hazard type 1 - find hazard spots and check not within distance of other hazard
	while (mTileManager.H1 > 0) { // until all hazards have been placed repeat (may cause issue )
		for (int i = 0; i < mTileManager.mDimention; i++) {
			for (int o = 0; o < mTileManager.mDimention; o++) {
				// calculate random variable to figure out hazard spot
				int r = rand() % mTileManager.dimSquare; // favors the lower numbers
				//int r = grid(rng); // causes a worse problem
				if (r <= 10 && mTileManager.H1 > 0) { // if tile is selected
					// DISTANCE CHECK HERE
					// main is central square (number to square)
					//			  o
					//	 o		 ooo
					//	ooo		ooooo
					//	 o		 ooo
					//			  o
					bool SAFE = true;
					int Cycles = (mTileManager.H1Dist * 2) + 1; // max number / for loop
					bool odd = true; // whether or not to run odd or even logic
					int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
					int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
					for (int d = 0; d < Cycles; d++) {
						bool set = false;
						if (odd == true) { // larger loop
							for (int x = 0; x < (mTileManager.H1Dist + 1); x++) {
								// xgrid = i -H1Dist + Xcycle + x
								// ygrid = o + Ycycle - x
								int checkx = (i - mTileManager.H1Dist + Xcycle + x);
								int checky = (o + Ycycle - x);
								if ((i - mTileManager.H1Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - mTileManager.H1Dist + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x)
									< mTileManager.mDimention)
									if (coords[i - mTileManager.H1Dist + Xcycle + x][o + Ycycle - x] == 5)
										SAFE = false;
							}
							Xcycle++;
						} // X ISNT CALCULATED PROPERLY, H1DIST + 1 ISN'T WORKING
						if (odd == false) { // smaller loop
							for (int x = 0; x < mTileManager.H1Dist; x++) {
								// xgrid = i(-H1Dist+1) + Xcycle + x
								// ygrid = o + Ycyle - x
								int checkx = (i - mTileManager.H1Dist + Xcycle + x);
								int checky = (o + Ycycle - x);
								if ((i - mTileManager.H1Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - mTileManager.H1Dist + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x)
									< mTileManager.mDimention)
									if (coords[i - mTileManager.H1Dist + Xcycle + x][o + Ycycle - x] == 5)
										SAFE = false;
							}
							Ycycle++;
						}
						if (odd == true && set == false) {
							odd = false;
							set = true;
						}
						if (odd == false && set == false) {
							odd = true;
							set = true;
						}
					}

					if (SAFE == true) {
						coords[i][o] = 5;
						origin[i][o] = 5; // 
						mTileManager.H1 -= 1;
					}
				}
			}
		}
		//H1 -= 1;
	}

	// hazard type 2 - find hazard spots and check not within distance of other hazard
	while (mTileManager.H2 > 0) { // until all hazards have been placed repeat (may cause issue )
		for (int i = 0; i < mTileManager.mDimention; i++) {
			for (int o = 0; o < mTileManager.mDimention; o++) {
				// calculate random variable to figure out hazard spot
				int r = rand() % mTileManager.dimSquare;
				//int r = grid(rng); // causes worse problems
				if (r <= 10 && mTileManager.H2 > 0) { // if tile is selected
					// DISTANCE CHECK HERE
					// main is central square (number to square)
					//			  o
					//	 o		 ooo
					//	ooo		ooooo
					//	 o		 ooo
					//			  o
					bool SAFE = true;
					int Cycles = (mTileManager.H2Dist * 2) + 1; // max number / for loop
					bool odd = true; // whether or not to run odd or even logic
					int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
					int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
					for (int d = 0; d < Cycles; d++) {
						bool set = false;
						if (odd == true) { // larger loop
							for (int x = 0; x < (mTileManager.H2Dist + 1); x++) {
								// xgrid = i -H1Dist + Xcycle + x
								// ygrid = o + Ycycle - x
								if ((i - mTileManager.H2Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - mTileManager.H2Dist + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x)
									< mTileManager.mDimention)
									if (coords[i - mTileManager.H2Dist + Xcycle + x][o + Ycycle - x] == 6)
										SAFE = false;
							}
							Xcycle++;
						}
						if (odd == false) { // smaller loop
							for (int x = 0; x < mTileManager.H2Dist; x++) {
								// xgrid = i(-H1Dist+1) + Xcycle + x
								// ygrid = o + Ycyle - x
								if ((i - mTileManager.H2Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - mTileManager.H2Dist + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x)
									< mTileManager.mDimention)
									if (coords[i - mTileManager.H2Dist + Xcycle + x][o + Ycycle - x] == 6)
										SAFE = false;
							}
							Ycycle++;
						}
						if (odd == true && set == false) {
							odd = false;
							set = true;
						}
						if (odd == false && set == false) {
							odd = true;
							set = true;
						}
					}

					if (SAFE == true) {
						coords[i][o] = 6;
						origin[i][o] = 6;
						mTileManager.H2 -= 1;
					}
				}
			}
		}
	}

	// hazard type 3 - find hazard spots and check not within distance of other hazard
	while (mTileManager.H3 > 0) { // until all hazards have been placed repeat (may cause issue )
		for (int i = 0; i < mTileManager.mDimention; i++) {
			for (int o = 0; o < mTileManager.mDimention; o++) {
				// calculate random variable to figure out hazard spot
				int r = rand() % mTileManager.dimSquare;
				//int r = grid(rng); // causes a worse problem
				if (r <= 10 && mTileManager.H3 > 0) { // if tile is selected
					// DISTANCE CHECK HERE
					// main is central square (number to square)
					//			  o
					//	 o		 ooo
					//	ooo		ooooo
					//	 o		 ooo
					//			  o
					bool SAFE = true;
					int Cycles = (mTileManager.H3Dist * 2) + 1; // max number / for loop
					bool odd = true; // whether or not to run odd or even logic
					int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
					int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
					for (int d = 0; d < Cycles; d++) {
						bool set = false;
						if (odd == true) { // larger loop
							for (int x = 0; x < (mTileManager.H3Dist + 1); x++) {
								// xgrid = i -H1Dist + Xcycle + x
								// ygrid = o + Ycycle - x
								if ((i - mTileManager.H3Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - mTileManager.H3Dist + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x)
									< mTileManager.mDimention)
									if (coords[i - mTileManager.H3Dist + Xcycle + x][o + Ycycle - x] == 4)
										SAFE = false;
							}
							Xcycle++;
						}
						if (odd == false) { // smaller loop
							for (int x = 0; x < mTileManager.H3Dist; x++) {
								// xgrid = i(-H1Dist+1) + Xcycle + x
								// ygrid = o + Ycyle - x
								if ((i - mTileManager.H3Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - mTileManager.H3Dist + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x)
									< mTileManager.mDimention)
									if (coords[i - mTileManager.H3Dist + Xcycle + x][o + Ycycle - x] == 4)
										SAFE = false;
							}
							Ycycle++;
						}
						if (odd == true && set == false) {
							odd = false;
							set = true;
						}
						if (odd == false && set == false) {
							odd = true;
							set = true;
						}
					}

					if (SAFE == true) {
						coords[i][o] = 4;
						origin[i][o] = 4;
						mTileManager.H3 -= 1;
					}
				}
			}
		}
		//H1 -= 1;
	}

	// set range of hazard using min & max size and set the nearby tiles as such

	// COULD TRY TO IMPLEMENT RANDOMNESS HERE AS WELL, SAVE SPACE AND 

	for (int i = 0; i < mTileManager.mDimention; i++) {
		for (int o = 0; o < mTileManager.mDimention; o++) {
			// use origin to select central tiles & set all tiles within gen 
			if (origin[i][o] == 5) { // if central tile found
				int haz1 = gen1(rng); // random between min & max
				// use for loop to set all tiles within gen1 to hazard
				int Cycles = (int(haz1) * 2) + 1; // max number / for loop
				bool odd = true; // whether or not to run odd or even logic
				int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (haz1 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - haz1 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz1 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x)
								< mTileManager.mDimention) // error catcher
								coords[i - haz1 + Xcycle + x][o + Ycycle - x] = 5; // sets coord to hazard
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < haz1; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							int checkx = (i - haz1 + Xcycle + x);
							int checky = (o + Ycycle - x);
							if ((i - haz1 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz1 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								coords[i - haz1 + Xcycle + x][o + Ycycle - x] = 5;
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
				// RANDOMNESS HERE
				// use H1RandomDist as max distance from central AND randomness is the range within dimsquare to use?

				int Rand1 = mTileManager.H1RandomDist + mTileManager.H1Dist; // set
				// use for loop to set all tiles within gen1 to hazard
				Cycles = (Rand1 * 2) + 1;
				odd = true; // whether or not to run odd or even logic
				Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (Rand1 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - Rand1 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand1 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								if (coords[i - Rand1 + Xcycle + x][o + Ycycle - x] != 5) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % mTileManager.dimSquare);
									if (use <= mTileManager.H1Random)
										coords[i - Rand1 + Xcycle + x][o + Ycycle - x] = 5;
								}
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < Rand1; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							//int checkx = (i - haz1 + Xcycle + x); // DEBUG
							//int checky = (o + Ycycle - x); // DEBUG
							if ((i - Rand1 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand1 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								if (coords[i - Rand1 + Xcycle + x][o + Ycycle - x] != 5) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % mTileManager.dimSquare);
									if (use <= mTileManager.H1Random)
										coords[i - Rand1 + Xcycle + x][o + Ycycle - x] = 5;
								}
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
			}
			// HAZARD 2
			if (origin[i][o] == 6) { // if central tile found
				int haz2 = gen2(rng); // random between min & max
				// use for loop to set all tiles within gen1 to hazard
				int Cycles = (int(haz2) * 2) + 1; // max number / for loop
				bool odd = true; // whether or not to run odd or even logic
				int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (haz2 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - haz2 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz2 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								coords[i - haz2 + Xcycle + x][o + Ycycle - x] = 6; // sets coord to hazard
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < haz2; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							if ((i - haz2 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz2 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								coords[i - haz2 + Xcycle + x][o + Ycycle - x] = 6;
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
				// RANDOMNESS HERE
				// use H1RandomDist as max distance from central AND randomness is the range within dimsquare to use?

				int Rand2 = mTileManager.H2RandomDist + mTileManager.H2Dist; // set
				// use for loop to set all tiles within gen1 to hazard
				Cycles = (Rand2 * 2) + 1;
				odd = true; // whether or not to run odd or even logic
				Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (Rand2 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - Rand2 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand2 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								if (coords[i - Rand2 + Xcycle + x][o + Ycycle - x] != 6) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % mTileManager.dimSquare);
									if (use <= mTileManager.H2Random)
										coords[i - Rand2 + Xcycle + x][o + Ycycle - x] = 6;
								}
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < Rand2; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							//int checkx = (i - haz1 + Xcycle + x); // DEBUG
							//int checky = (o + Ycycle - x); // DEBUG
							if ((i - Rand2 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand2 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								if (coords[i - Rand2 + Xcycle + x][o + Ycycle - x] != 6) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % mTileManager.dimSquare);
									if (use <= mTileManager.H2Random)
										coords[i - Rand2 + Xcycle + x][o + Ycycle - x] = 6;
								}
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
			}
			// HAZARD 3
			if (origin[i][o] == 4) { // if central tile found
				int haz3 = gen3(rng); // random between min & max
				// use for loop to set all tiles within gen1 to hazard
				int Cycles = (int(haz3) * 2) + 1; // max number / for loop
				bool odd = true; // whether or not to run odd or even logic
				int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (haz3 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - haz3 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz3 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								coords[i - haz3 + Xcycle + x][o + Ycycle - x] = 4; // sets coord to hazard
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < haz3; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							if ((i - haz3 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz3 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								coords[i - haz3 + Xcycle + x][o + Ycycle - x] = 4;
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
				// RANDOMNESS HERE
				// use H1RandomDist as max distance from central AND randomness is the range within dimsquare to use?

				int Rand3 = mTileManager.H3RandomDist + mTileManager.H3Dist; // set
				// use for loop to set all tiles within gen1 to hazard
				Cycles = (Rand3 * 2) + 1;
				odd = true; // whether or not to run odd or even logic
				Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (Rand3 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - Rand3 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand3 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								if (coords[i - Rand3 + Xcycle + x][o + Ycycle - x] != 4) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % mTileManager.dimSquare);
									if (use <= mTileManager.H3Random)
										coords[i - Rand3 + Xcycle + x][o + Ycycle - x] = 4;
								}
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < Rand3; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							//int checkx = (i - haz1 + Xcycle + x); // DEBUG
							//int checky = (o + Ycycle - x); // DEBUG
							if ((i - Rand3 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand3 + Xcycle + x) < mTileManager.mDimention && (o + Ycycle - x) 
								< mTileManager.mDimention) // error catcher
								if (coords[i - Rand3 + Xcycle + x][o + Ycycle - x] != 4) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % mTileManager.dimSquare);
									if (use <= mTileManager.H3Random)
										coords[i - Rand3 + Xcycle + x][o + Ycycle - x] = 4;
								}
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
			}
		}
	}

	// use randomness attributed to that hazard to activate / deactivate other tiles outside up to range of 2 & 
	// interior tiles, use gen value + 2


	// 4 = ICE, 5 = POISON/MARSH, 6 = MUD

	//setup data array
	for (int k = 0; k < mTileManager.mDimention; ++k)
	{
		mTileManager.mMapData.push_back(std::vector<mapData>());

		for (int j = 0; j < mTileManager.mDimention; ++j)
		{
			mTileManager.mMapData.back().push_back(mapData());

			if (coords[k][j] <= 0)
				mTileManager.mMapData.back().back().texIndex = rand() % 2; // random plain tile
			else
				mTileManager.mMapData.back().back().texIndex = coords[k][j]; // set as hazard

			//if (k == 0 || j == 0 || k == 31 || j == 31) { // DEBUG CODE, NEEDS REMOVING AFTER TESTING FINISHED
			//	mMapData.back().back().texIndex = 5;
			//} // 4 = ICE, 5 = POISON/MARSH, 6 = MUD
			//else {
			//	mMapData.back().back().texIndex = rand() % 2; // TODO: NOTE: TEXTURE IS RANDOMIZED HERE
			//}
		}
	}

	// create tiles
	mTileManager.mTileGrid.reserve(mTileManager.mDimention);
	for (int k = 0; k < mTileManager.mDimention; ++k)
	{
		TileLine tl;
		tl.reserve(mTileManager.mDimention);

		for (int j = 0; j < mTileManager.mDimention; ++j)
		{
			tl.push_back(Tile());
			tl.back().Initialize(mTileManager.mRenderItemName); //Creates instances

			// Position instanced along a 3D grid.
			tl.back().mpInstance->World = DirectX::XMFLOAT4X4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				mTileManager.x + j * mTileManager.dx, 0.0f, mTileManager.z + k * mTileManager.dz, 1.0f);
			// TODO: NEED TO REMOVE THE FIRST TEXTURE FROM THE OBJECTS FIRST
			
			//Selects random index from array of textures WIP - will be using auto generated clumping 2d array
			tl.back().mpInstance->MaterialIndex = mTileManager.mMapData[j][k].texIndex; // set tile texture to struct data
			

			// set all tiles to a default texture, this can then be used to determine the tile type
			//tl.back().mpInstance->MaterialIndex = 0; //0 = MOSS STONE BRICK, 1 = MOSS STONE, 2 LAB TILES,
			//3 = DEFUALT CRATE, 4 = ICE, 5 = POISON/MARSH, 6 = MUD, 7 = black now (was white), 8 = BLACK ONWARDS
		}

		mTileManager.mTileGrid.push_back(std::move(tl));

	}
}

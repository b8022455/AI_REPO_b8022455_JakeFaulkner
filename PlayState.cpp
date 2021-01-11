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
			t.mpInstance->MaterialIndex = 0/*GameApp::Get().GetMaterialIndex("tileTex")*/;
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

	mPlants.reserve(20);
	
	// ui bar
	mPlayerHealthBar.Initialise(GC::BAR_GRN, GC::BAR_RED);
	mPlayerHealthBar.SetMinMax(0, mPlayer.health); // todo change to max health
	mPlayerHealthBar.SetValue(mPlayer.health);
	mPlayerHealthBar.SetPosition({ 200.0f, 20.0f }); // todo add values to constants.h

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
		e.Update(gt); 
		//Enemy look at players position (Currently Working On)
		if (mPlayer.GetPos().x > e.GetPos().x)
			if (mPlayer.GetPos().z > e.GetPos().z)		//X and Z are bigger
				e.SetRotationY(90);
			else										//X is bigger
				e.SetRotationY(180);
		else											//X is not bigger
		{
			if (mPlayer.GetPos().z > e.GetPos().z)
				e.SetRotationY(0);
			else										//X and Z are not bigger
				e.SetRotationY(-90);
		}

		if (mCombatController.CheckCollision(mPlayer.GetPos(), e.GetPos()))
		{
			mPlayer.DamagePlayer(e.GetAttack());
			mPlayerHealthBar.SetValue(mPlayer.health);
			GameApp::Get().GetAudio().Play("playerHit01", nullptr, false, 1.0f,GetRandomVoicePitch());
		}

		if (mCombatController.CheckCollision(mPlayerWeapon.GetPos(), e.GetPos()))
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
		mPlayerHealthBar.SetValue(mPlayer.health);
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
	// UI

	std::for_each(mSprites.begin(), mSprites.end(), [](auto& sp)
	{
		sp.second.Draw();
	});

	mPlayerHealthBar.Draw();

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

			// Plant
			if (Input::Get().KeyReleased('7'))
			{
				CreatePlant();
			}


			if (Input::Get().KeyReleased('8'))
			{
				HarvestByRadius();
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
	
	mTileManager.REGEN();
}

void PlayState::UiUpdate(const GameTimer & gt)
{
	mSprites["testSpriteFirst"].rotation = cosf(gt.TotalTime()) * 0.1f;
	mSprites["testSpriteSecond"].rotation = sinf(gt.TotalTime());

	mPlayerHealthBar.Update(gt);
}

void PlayState::CreatePlant()
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
			}
		}
	}
}

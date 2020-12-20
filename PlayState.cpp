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
			t.mpInstance->MaterialIndex = 6;
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

	//mTile.Initialize("Tiles");
	mTileManager.Initialize();

	//Inventory.push_back({ "Potion" });		//Testing Item usage
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
		mEnemies.push_back(Enemy("EnemyType1"));
		mEnemies.push_back(Enemy("EnemyType1"));
		mEnemies.push_back(Enemy("EnemyType1"));

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

	// TODO: (NOTE) CHECK AREA HAZARDS HERE
	// if player.x & player.z are within the tile.x & tile.z, cause effects, for now DEBUG logic

	// reset hazard timer for tile hazards 
	if (mPlayer.hazardTimer >= 0) {
		mPlayer.hazardTimer -= gt.DeltaTime();
	}

	// damage player over time, needs a wait between effect activations
	if (mPlayer.GetPos().x >= 10.0f) {
		if (mPlayer.hazardTimer <= 0) { // if hazard should be active
			mPlayer.health -= 5;
			mPlayer.hazardTimer = 3; // reset hazard timer
		}
	}

	// Game Camera follows player
	mCameras.at(CAMERA_TYPE::GAME).SetPosition(
		Lerp(mCameras.at(CAMERA_TYPE::GAME).GetPosition(), mPlayer.GetPos() + CAM_OFFSET,  0.9999f * gt.DeltaTime())
	);

	// slow player until they leave the tile
	// while player is not on a slow tile movespeed is normal? (for possible use when applied to tile object)
	// if player is on a slow tile
	if (mPlayer.GetPos().z >= 10.0f) {
		mPlayer.Slowed = true;
	}
	// if player is not on a slow tile
	if ((mPlayer.GetPos().z <= 10.0f) && mPlayer.Slowed == true) {
		mPlayer.Slowed = false;
	}
	

	int i = 0;
	std::for_each(mEnemies.begin(), mEnemies.end(), [&](Enemy& e)
	{ 
		e.Update(gt); 
		if (mCombatController.CheckCollision(mPlayer.GetPos(), e.GetPosition()))
		{
			float x = 5.0f;
			mPlayer.DamagePlayer(5);
			/*mCamera.Strafe(-x * gt.DeltaTime());
			mCamera.UpdateViewMatrix();*/
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
				mEnemies.push_back(Enemy("EnemyType1"));
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

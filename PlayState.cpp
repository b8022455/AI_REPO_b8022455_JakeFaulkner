#include "XmfloatOverload.h"
#include "PlayState.h"
#include "GameApp.h"
#include "XmfloatOverload.h"
#include "SimpleMath.h"
//#include "Input.h"
#include "Constants.h"
#include <unordered_map>


using ButtonState = GamePad::ButtonStateTracker::ButtonState;


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

	Inventory.push_back({ "Potion" });		//Testing Item usage

	for (auto& c : mCameras)
	{
		c.SetPosition(0.0f, 25.0f, 0.0f);
		c.Pitch(XMConvertToRadians(CAM_ANGLE)); // SETS CAMERA TO FACE DOWN
	}

	//GameApp::Get().OnResize();

	mPlayer.Initialize("Player");					// todo adapt GameApp mPlayer to this state
	mPlayerWeapon.Initialize("Weapon");
	
	int n(0);
	int clumpSize(5);
	DirectX::XMFLOAT3 position = mTile.SetRandom();

	while (n <= clumpSize)
	{
		mTile.Initialize("Tiles");
		{
			mTile.mpInstance->MaterialIndex = 4;
			//mTile.SetRandomPosition();			
			mTile.CreateTileWallHorizontal(clumpSize, position, n);
			//mTile.CreateTileWallVertical(clumpSize, position, n);
		}
		n++;
	}


	// Setup temp enemies
	{
		// inserts n of enemies
		mEnemies.push_back(Enemy(0));
		mEnemies.push_back(Enemy(0));
		mEnemies.push_back(Enemy(0));

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
			e.DamageEnemy(25);		//Takes away health from enemy + blowsback enemy position
			if (e.GetHealth() < 0)
			{
				// gain exp
				mExperience.AddExp(GC::EXP_DEFAULT); 

				//Could be put into an exists function in Inventory Class
				Item droppedItem = e.GetDropItem();
				if (droppedItem.name != "Empty")		//I.e An item was dropped from enemy
				{
					bool itemExists = false;
					for (size_t i = 0; i < Inventory.size(); i++)
					{
						if (Inventory.at(i).name == droppedItem.name)
						{
							Inventory.at(i).amount++;
							itemExists = true;
						}
					}

					if (!itemExists)
						Inventory.push_back({ droppedItem });
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
	if (((btnState & MK_LBUTTON) != 0))
	{

		POINT lastMousePos = GameApp::Get().GetLastMousePos();

		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - lastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - lastMousePos.y));

		mCameras.at(CAMERA_TYPE::GAME).Pitch(dy);
		mCameras.at(CAMERA_TYPE::GAME).RotateY(dx);
	}
}

void PlayState::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void PlayState::OnKeyboardInput(const GameTimer & gt)
{

	Controls(gt); 


	

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

	float moveSpeed = 5.0f;
	float zoomSpeed = 20.0f;

	bool playerMoved = false;

	//Gamepad
	if (Input::Get().GamePadConnected())
	{

		// Left stick
		mPlayer.Move(  gt, Input::Get().LeftStickXZ()* moveSpeed);

		// Attack
		if (Input::Get().GamePad().rightTrigger == ButtonState::HELD)
		{
			mCombatController.PlayerAttack();
		}

		if (Input::Get().GamePad().start == ButtonState::PRESSED)
		{
			GameApp::Get().ChangeState("PauseMenu");
		}

		if (Input::Get().GamePad().y == ButtonState::HELD)
		{
			GameApp::Get().mDebugLog << "Inventory:  (size " << Inventory.size() << ")";

			for (auto& i : Inventory)
				GameApp::Get().mDebugLog << i.name << " : " << i.amount << "\n";
		}
		//todo example remove
		GameApp::Get().mDebugLog << " \n" << Input::Get().LeftStickF2().x << "  " << Input::Get().LeftStickF2().y;
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
				mCameras.at(CAMERA_TYPE::DEBUG).Walk(zoomSpeed * dt);
			}

			if (Input::Get().KeyHeld(GC::KEY_BK))
			{
				mCameras.at(CAMERA_TYPE::DEBUG).Walk(-zoomSpeed * dt);
			}

			if (Input::Get().KeyHeld(GC::KEY_RT))
			{
				mCameras.at(CAMERA_TYPE::DEBUG).Strafe(moveSpeed * dt);
			}

			if (Input::Get().KeyHeld(GC::KEY_LT))
			{
				mCameras.at(CAMERA_TYPE::DEBUG).Strafe(-moveSpeed * dt);
			}

			if (Input::Get().KeyHeld(GC::KEY_RAISE))
			{
				mCameras.at(CAMERA_TYPE::DEBUG).Elevate(moveSpeed * dt);
			}

			if (Input::Get().KeyHeld(GC::KEY_LOWER))
			{
				mCameras.at(CAMERA_TYPE::DEBUG).Elevate(-moveSpeed * dt);
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
				mEnemies.push_back(Enemy());
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
			inventoryPosition++;
			if (inventoryPosition >= Inventory.size())
				inventoryPosition = 0;						//Loop back round to top of inventory
		}

		if (Input::Get().KeyReleased(VK_UP) & itemMenuOpen)
		{
			inventoryPosition--;
			if (inventoryPosition < 0)
				inventoryPosition = (int)Inventory.size() - 1;	//Loop back round to bottom of inventory
		}

		//Opens and closes item menu
		if (Input::Get().KeyReleased(GC::KEY_INVENTORY))
		{
			if (!itemMenuOpen)
				itemMenuOpen = true;		//When the item menu UI pops up on screen
			else
				itemMenuOpen = false;
		}

		if (itemMenuOpen)
		{
			GameApp::Get().mDebugLog << "Inventory:  (size " << Inventory.size() << ")\n";

			//Debug purposes: shows which items are in inventory
			for (size_t i = 0; i < Inventory.size(); i++)
				GameApp::Get().mDebugLog << Inventory.at(i).name << " x" << Inventory.at(i).amount << "\n";

			//Debug purposes: shows the currently selected item based on inventoryPosition value
			if (Inventory.size() > 0)
				GameApp::Get().mDebugLog << "Current Selected Item: " << Inventory.at(inventoryPosition).name << " x" << Inventory.at(inventoryPosition).amount << "\n";
		}
	}

	//Use Item Key, change to something else, or only allow when on item/pause menu
	if (Input::Get().KeyReleased('U') & itemMenuOpen)
	{
		if (Inventory.size() > 0)
		{
			if (Inventory.at(inventoryPosition).category == ItemCategory::Healing)		//Only items that can be used are healing items (Equipping weapons might be a diff key)
			{
				std::string itemName = Inventory.at(inventoryPosition).name;
				//mPlayer.UseItem(itemName);			//Function that has switch containing item names and uses
				Inventory.at(inventoryPosition).amount--;

				if (Inventory.at(inventoryPosition).amount <= 0)
				{
					Inventory.erase(Inventory.begin() + inventoryPosition);		//Remove selected item if amount has run out
					if (inventoryPosition >= Inventory.size())					//If selected item was last item in list, reposition selected item value to prevent going out of vector bounds
						inventoryPosition = 0;
				}
			}
		}
	}
}
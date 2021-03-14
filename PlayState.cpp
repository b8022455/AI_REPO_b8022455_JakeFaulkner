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

void PlayState::InitializeTraders()
{

	mTraders.reserve(10);

	mTraders.push_back(Trader(GC::TRADER_NAME_TEST, GC::TRADER_NAME_TEST, GC::TRADER_NAME_TEST));
	mTraders.push_back(Trader(GC::TRADER_NAME_1, GC::TRADER_NAME_1, GC::TRADER_NAME_1));
	mTraders.push_back(Trader(GC::TRADER_NAME_2, GC::TRADER_NAME_2, GC::TRADER_NAME_2));
	mTraders.push_back(Trader(GC::TRADER_NAME_3, GC::TRADER_NAME_3, GC::TRADER_NAME_3));
	mTraders.push_back(Trader(GC::TRADER_NAME_1, GC::TRADER_NAME_2, GC::TRADER_NAME_3));

	//sets up a line of traders
	for (auto& t : mTraders)
	{
		t.Initialize(GC::GO_TRADER);
		t.mpInstance->MaterialIndex = 0;
	}

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
			distanceA = SimpleMath::Vector3::Distance(from, a.GetPosV3());
			distanceB = SimpleMath::Vector3::Distance(from, b.GetPosV3());

			return distanceA < distanceB;
		}
	private:
		float distanceA = 0.0f, distanceB = 0.0f;
	};

	SortByDistance s;
	// distance from player location
	s.from = mPlayer.GetPosV3();

	std::sort(mTraders.begin(), mTraders.end(), std::ref(s));

	float distance = SimpleMath::Vector3::Distance(mPlayer.GetPosV3(), mTraders.front().GetPosV3());

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

Enemy PlayState::Spawn(std::string enemyType) // USED TO INITIALISE ENEMIES
{
	if (enemyType == GC::ENEMY_TYPE_1)
	{
		Enemy e(GC::ENEMY_TYPE_1, GC::ENEMYTYPE1_ATTACK);
		e.Initialize("EnemyGhoul");
		e.SetHealth(GC::ENEMYTYPE1_HEALTH);
		e.SetRandomPosition();
		e.particles.resize(20);
		return e;
	}
	else if (enemyType == GC::ENEMY_TYPE_2)
	{
		Enemy e(GC::ENEMY_TYPE_2, GC::ENEMYTYPE2_ATTACK);
		e.Initialize(GC::GO_ENEMY);
		e.SetHealth(GC::ENEMYTYPE2_HEALTH);
		e.SetRandomPosition();
		return e;
	}
	else {
		assert(enemyType != GC::ENEMY_TYPE_1 && enemyType != GC::ENEMY_TYPE_2);
		return Enemy(GC::ENEMY_TYPE_1, 15);		//Default return parameter
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

	mTileManager.Initialize();

	timeSet();

	mPlayer.Initialize("Player");
	mPlayerWeapon.Initialize("Weapon");

	mPlants.reserve(20);

	// ui bar
	mPlayerHealthBar.Initialise(GC::BAR_GRN, GC::BAR_RED);

	//InitializeTraders();

	mAlgorithm.CreateInitialPopulation();

	mCombatController.Initialize(&mPlayer, &mPlayerWeapon, &mAlgorithm.mPopulation);

	// Sprites
	{
		const DirectX::XMINT2 s = GameApp::Get().GetClientSizeU2(); //todo on resize
		SimpleMath::Vector2 v = GameApp::Get().GetClientSize();

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
				s.x + (s.x / 3) + 128,			//  third of viewport plus offset
				s.y - 10						// padding
			};

			mInventoryPanel.Initialize("uiTex", src, dst);
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
			mHelpText.color = DirectX::Colors::White;
			mHelpText.fontIndex = 1;
			mHelpText.string = "(W)(A)(S)(D) - Movement \n(Spacebar) - attack \n(Enter) - Opens/Closes inventory \n(P) - Pauses the game \n(Q) - Trades when near a trader \n(E) - Use an item when in inventory";
		}

		//Help text
		{
			mHelpMessage.mText.center = true;
			mHelpMessage.mText.position = DirectX::SimpleMath::Vector2{ 40.f, 550.f };
			mHelpMessage.mText.color = DirectX::Colors::Red;
			mHelpMessage.Activate(GC::HELP_MESSAGES[2], 6.0f);

			mTradeHelpMessage.mText.center = true;
			mTradeHelpMessage.mText.position = DirectX::SimpleMath::Vector2{ 250.f, 150.f };
			mTradeHelpMessage.mText.color = DirectX::Colors::Red;

			mGenerationMessage.mText.center = true;
			mGenerationMessage.mText.position = DirectX::SimpleMath::Vector2(400.f, 300.f);
			mGenerationMessage.mText.color = DirectX::Colors::Red;

		}
	}

	//Instantiates enemies, inventory, trader positions
	Reset();

	// needed in init for dirty frame
	for (auto& c : mCameras)
	{
		c.SetPosition(0.0f, 25.0f, 0.0f);
		c.Pitch(XMConvertToRadians(CAM_ANGLE)); // SETS CAMERA TO FACE DOWN
		c.UpdateViewMatrix();
	}

}

void PlayState::reInitialize() { // USED TO LOAD A NEW MAP & ENEMIES, ETC, WHEN LEAVING AN AREA

	ReGen();
	//eGen(false);
	timeSet();

	timeChange = 0.0f;

	mCombatController.Initialize(&mPlayer, &mPlayerWeapon, &mAlgorithm.mPopulation);

	mPlayer.AreaClear = false;
	mPlayer.genArea = false;
	if (mPlayer.GetPos().x >= GC::PLAYER_RIGHTBOUND - 0.9375f) { mPlayer.SetPos({ -14.0f,0.0f,0.0f }); }
	if (mPlayer.GetPos().x >= GC::PLAYER_LEFTBOUND + 0.9375f) { mPlayer.SetPos({ 14.0f,0.0f,0.0f }); }
	if (mPlayer.GetPos().z >= GC::PLAYER_UPBOUND - 0.9375f) { mPlayer.SetPos({ 0.0f,0.0f,-14.0f }); }
	if (mPlayer.GetPos().z >= GC::PLAYER_DOWNBOUND + 0.9375f) { mPlayer.SetPos({ 0.0f,0.0f,14.0f }); }
}

void PlayState::timeSet() {
	PassConstants* pMainPassCB = GameApp::Get().GetMainPassCB();
	//GameApp::Get().setPhase(timeCycle);
	if (timeCycle == 1) {
		pMainPassCB->AmbientLight = GC::DAWN_COLOUR; // dawn
		pMainPassCB->Lights[1].Strength = GC::DAWN_STRENGTH;
	}
	if (timeCycle == 2) {
		pMainPassCB->AmbientLight = GC::NOON_COLOUR; // noon
		pMainPassCB->Lights[1].Strength = GC::NOON_STRENGTH;
	}
	if (timeCycle == 3) {
		pMainPassCB->AmbientLight = GC::EVENING_COLOUR; // evening
		pMainPassCB->Lights[1].Strength = GC::EVENING_STRENGTH;
	}
	if (timeCycle == 4) {
		pMainPassCB->AmbientLight = GC::PITCH_COLOUR; // pitch
		pMainPassCB->Lights[1].Strength = GC::PITCH_STRENGTH;
	}
}

void PlayState::Update(const GameTimer& gt)
{
	//Genetic Algorithm - Check if all enemies have been defeated in the game
	if (mAlgorithm.CurrentGenerationFinished())
	{
		mGenerationMessage.Activate("New Generation", 1.5f);
		mAlgorithm.SelectCandidates();

		for (auto& e : mAlgorithm.mPopulation)
		{
			while (DirectX::SimpleMath::Vector3::Distance(mPlayer.GetPos(), e.GetPos()) < 8.f)	//Prevents enemies from spawning inside a trader
				e.SetRandomPosition();
		}
	}

	//mTileManager.Update(gt);
	mCombatController.Update(gt);

	switch (GameApp::Get().menusShown)
	{
	case GC::NO_TUTORIAL_VALUE:
		shownAttackTutorial = true;
		shownInventoryUseTutorial = true;
		shownPlantTutorial = true;
	case 0:
		GameApp::Get().mTutorialText = GC::TUTORIAL_INTRO;
		GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
		break;

	case 1:
		GameApp::Get().mTutorialText = GC::TUTORIAL_OBJECTIVE;
		GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
		break;

	case 2:
		GameApp::Get().mTutorialText = GC::TUTORIAL_MOVE;
		GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
		break;

	case 3:
		GameApp::Get().mTutorialText = GC::TUTORIAL_TRADE;
		GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
		break;

	case 4:
		GameApp::Get().mTutorialText = GC::TUTORIAL_INVENTORY;
		GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
		break;
	}

	if (revolvingHintTimer.HasTimeElapsed(gt.DeltaTime(), 8.f))
	{
		mHelpMessage.mText.center = false;
		mHelpMessage.mText.position = DirectX::SimpleMath::Vector2{ 250.f, 550.f };
		mHelpMessage.mText.color = DirectX::Colors::Red;
		mHelpMessage.Activate(GC::HELP_MESSAGES[revolvingHintPosition], 7.0f);
		revolvingHintPosition++;
		if (revolvingHintPosition > 8)
			revolvingHintPosition = 2;
	}

	if (playerName == "")		//Only does this once
		GetName();

	timeChange += gt.DeltaTime();
	
	// AFTER 2 MINUTES CHANGE TIME PHASE IN GAME (ONLY PARTICULARLY USEFUL DURING PITCH )
	if (timeChange >= 120.0f) {
		timeChange = 0.0f;
		timeCycle += 1;
		if (timeCycle > 4)
			timeCycle = 1;
		//eGen(false);
		timeSet();
	}

	//Transition to win state when collecting all items needed for the car
	if (TraderStoryComplete())
	{
		StoreScore();
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

	//Checking player remains in bounds when being attacked
	if (!mPlayer.WithinBounds(mPlayer.GetPos() + mPlayer.BouncebackPosition))
		mPlayer.BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	mPlayer.Update(gt);

	// reset hazard timer for tile hazards 
	if (mPlayer.hazardTimer >= 0) {
		mPlayer.hazardTimer -= gt.DeltaTime();
	}

	// Game Camera follows player
	mCameras.at(CAMERA_TYPE::GAME).SetPosition(
		Lerp(mCameras.at(CAMERA_TYPE::GAME).GetPosition(), mPlayer.GetPos() + CAM_OFFSET, 0.9999f * gt.DeltaTime())
	);

	for (int i = 0; i < mAlgorithm.mPopulation.size(); i++)
	{
		mAlgorithm.mPopulation.at(i).Update(gt);
		//e.Update(gt);


		if (mAlgorithm.mPopulation.at(i).mEnabled)
		{
			//Enemy look at players position (only do when in range), only look when not attacking either
			XMVECTOR playerPosition = XMLoadFloat3(&mPlayer.GetPos());

			if (DirectX::SimpleMath::Vector3::Distance(mPlayer.GetPos(), mAlgorithm.mPopulation.at(i).GetPos()) < mAlgorithm.mPopulation.at(i).GetEnemyRange())
			{
				mAlgorithm.mPopulation.at(i).LookAt(playerPosition);

				if (shownAttackTutorial == false)
				{
					GameApp::Get().mTutorialText = GC::TUTORIAL_ATTACK;
					GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
					shownAttackTutorial = true;
				}

				if(!mAlgorithm.mPopulation.at(i).GetIfCanAttack())
					mAlgorithm.mPopulation.at(i).mBehaviour = Enemy::Behaviour::CHASE;
				else
					mAlgorithm.mPopulation.at(i).mBehaviour = Enemy::Behaviour::NONE;
			}
			else
				mAlgorithm.mPopulation.at(i).mBehaviour = Enemy::Behaviour::NONE;
		}

		// enemy collision with player
		if (mPlayer.CheckCollision(mPlayer.GetPos(), mAlgorithm.mPopulation.at(i).GetPos()) && mAlgorithm.mPopulation.at(i).GetHealth() > 0)
		{
			mPlayer.DamagePlayer(mAlgorithm.mPopulation.at(i).GetAttack(), mAlgorithm.mPopulation.at(i), gt);
			mPlayerHealthBar.SetValue(mPlayer.health);

			mAlgorithm.IncrementFitnessValue(i);

			if (shownPlantTutorial == false)	//replace bool?
			{
				GameApp::Get().mTutorialText = GC::TUTORIAL_PLANT;
				GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
				shownPlantTutorial = true;
			}

			//Transition to game over state
			if (mPlayer.health <= 0)
			{
				GameApp::Get().ChangeState("GameOver");
				GameApp::Get().GetAudio().Play("playerDie", nullptr, false, 1.0f, GetRandomVoicePitch());
			}
			else
			{
				GameApp::Get().GetAudio().Play("playerHit01", nullptr, false, 1.0f, GetRandomVoicePitch());
			}
		}

		//When checking if enemy is in range, have this be in that section to prevent enemy particles from far from being checked
		for (auto& p : mAlgorithm.mPopulation.at(i).particles)
		{
			if (mPlayer.CheckCollision(mPlayer.GetPos(), p.GetPos()) && mAlgorithm.mPopulation.at(i).GetHealth() > 0)
			{
				mPlayer.DamagePlayer(mAlgorithm.mPopulation.at(i).GetAttack(), mAlgorithm.mPopulation.at(i), gt);
				mPlayerHealthBar.SetValue(mPlayer.health);

				mAlgorithm.IncrementFitnessValue(i);

				if (shownPlantTutorial == false)	//replace bool?
				{
					GameApp::Get().mTutorialText = GC::TUTORIAL_PLANT;
					GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
					shownPlantTutorial = true;
				}

				GameApp::Get().GetAudio().Play("playerHit01", nullptr, false, 1.0f, GetRandomVoicePitch());
				//Transition to game over state
				if (mPlayer.health <= 0)
				{
					GameApp::Get().ChangeState("GameOver");
					GameApp::Get().GetAudio().Play("playerDie", nullptr, false, 1.0f, GetRandomVoicePitch());
				}
				else
				{
					GameApp::Get().GetAudio().Play("playerHit01", nullptr, false, 1.0f, GetRandomVoicePitch());
				}
				break;
			}
		}

		if (mPlayerWeapon.CheckCollision(mPlayerWeapon.GetPos(), mAlgorithm.mPopulation.at(i).GetPos()))
		{
			// TODO: (URGENT) FIX ENEMY MODEL LEFT ON SCREEN

			mAlgorithm.mPopulation.at(i).DamageEnemy(mPlayer.attack);		//Takes away health from enemy + blowsback enemy position
			if (mAlgorithm.mPopulation.at(i).GetHealth() <= 0)
			{
				// gain exp
				mExperience.AddExp(GC::EXP_DEFAULT);
				score += GC::SCORE_ENEMY;

				//Could be put into an exists function in Inventory Class
				std::string droppedItem = mAlgorithm.mPopulation.at(i).GetDropItem();
				if (droppedItem != "Empty")		//I.e An item was dropped from enemy
				{
					++mInventory[droppedItem];
				}


				mAlgorithm.mPopulation.at(i).MoveOffScreen();
				mAlgorithm.mPopulation.at(i).mEnabled = false;
				if (timeCycle == 4)
					newEnemy = true;

				GameApp::Get().GetAudio().Play("EnemyDie1", nullptr, false, 1.0f, GetRandomVoicePitch());
			}
			else
			{
				GameApp::Get().GetAudio().Play("EnemyHit1", nullptr, false, 1.0f, GetRandomVoicePitch());
			}
		}

		//Checking enemy remains in bounds
		if (!mAlgorithm.mPopulation.at(i).WithinBounds(mAlgorithm.mPopulation.at(i).GetPos() + mAlgorithm.mPopulation.at(i).BouncebackPosition))
			mAlgorithm.mPopulation.at(i).BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		// enemy collides with traders
		for (auto& t : mTraders)
			if (mAlgorithm.mPopulation.at(i).CheckCollision(mAlgorithm.mPopulation.at(i).GetPos() + mAlgorithm.mPopulation.at(i).BouncebackPosition, t.GetPos()))		//If there is a collision between any of the traders and the bounceback position of the enemy
				mAlgorithm.mPopulation.at(i).BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		mAlgorithm.mPopulation.at(i).SetVelocity(mPlayer.GetPos(), gt);

		if (!mAlgorithm.mPopulation.at(i).mEnabled)
		{
			mAlgorithm.MoveEnemyFromPopulation(i);
			break;
		}
	}

	// TODO: (REMEMBER) METHOD OF REGENNING ENEMIES AFTER THEIR DEATH HERE
	if (newEnemy == true) {
		newEnemy = false;
		//eGen(true);
	}
	PassConstants* pMainPassCB = GameApp::Get().GetMainPassCB();

	//flashing red for low health
	if (mPlayer.health <= GC::PLAYER_LOW_HEALTH)
	{
		float strength = sin(gt.TotalTime() * 5.0f) * 0.5f + 0.5f;

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

	if (mHelpLocation.first.y < 500.f)
		mHelpMessage.Activate("", 1.0f);		//Deactivates other hints from showing while help menu is open

	// Sprite update

	if (mExperience.HasLeveledUp())
	{
		mMessage.Activate("LEVEL UP", 5.0f);
		score += GC::SCORE_LVLUP;
		mPlayer.health = GC::PLAYER_MAX_HEALTH;
		mPlayerHealthBar.SetValue(mPlayer.health);
	}

	// show/hide item menu
	if (itemMenuOpen)
	{
		if (shownInventoryUseTutorial == false)
		{
			GameApp::Get().mTutorialText = GC::TUTORIAL_USE;
			GameApp::Get().ChangeState(GC::STATE_TUTORIAL);
			shownInventoryUseTutorial = true;
		}
		mInventoryText.string = "Inventory\n";
		// Selected item

		const std::string selectedItem = (*inventoryPosition).first;
		
		// all inventory items. Selected item prefixed with '>'
		std::for_each(mInventory.begin(), mInventory.end(), [&](auto& inv)
		{
			if (inventoryPosition != mInventory.end())
			{

				if ((*inventoryPosition).first == inv.first)
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

		// Display weapon attack power
		if (GC::ITEM_LIST.at(selectedItem).category == ItemCategory::WEAPON)
		{
			mInventoryText.string += "\nAttack +" + std::to_string(mPlayerWeapon.GetWeaponStats(selectedItem)) + "\n";
		}

		//Help instructions for inventory
		mHelpMessage.mText.center = true;
		mHelpMessage.mText.position = DirectX::SimpleMath::Vector2{ 650.f, 450.f };
		mHelpMessage.mText.color = DirectX::Colors::White;
		mHelpMessage.Activate(GC::HELP_MESSAGES[0], 0.1f);
	}


	// final goal is to reach a safe house, need to harvest a certain number of plants as payment to get in?

	//if (mPlayer.AreaClear && mPlayer.genArea) { // TODO: (REMEMBER) IMPLEMENT CHANGE STATE FOR NEW AREA HERE
	//	// change state, trigger regen
	//	areas += 1;
	//	timeCycle += 1;
	//	if (timeCycle > 4)
	//		timeCycle = 1;
	//	timeChange = 0.0f;
	//	if (areas < 4) {
	//		reInitialize();
	//		++GameApp::Get().mStoryIndex;
	//		GameApp::Get().ChangeState("Story"); //"NewArea1"
	//		//GameApp::Get().ChangeState(GC::STATE_NEW_AREA);
	//	}
	//	if (areas == 4) { // TODO: (REMEMBER) CURRENTLY WIN CONDITION IS HERE
	//		reInitialize();
	//		areas = 0;
	//		timeCycle = 1;
	//		StoreScore();
	//		GameApp::Get().ChangeState(GC::STATE_WIN);
	//	}
	//}

	// for dirty frame
	for (auto& c : mCameras)
	{
		c.UpdateViewMatrix();
	}
}

void PlayState::Draw(const GameTimer& gt)
{
	// UI

	mPlayerHealthBar.Draw();

	mInventoryPanel.Draw();
	mInventoryText.Draw();


	mMessage.Draw();
	mHelpMessage.Draw();
	mTradeHelpMessage.Draw();

	mHelpPanel.Draw();
	mHelpText.Draw();

	mScoreTextShadow.Draw();
	mScoreText.Draw();

	mGenerationMessage.Draw();

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

void PlayState::OnKeyboardInput(const GameTimer& gt)
{

	Controls(gt);

}

void PlayState::OnResume()
{
	// remove focus from trader
	mpActiveTrader = nullptr;

	GameAudio& ga = GameApp::Get().GetAudio();

	ga.Play("actionMusic", nullptr, true);

	// Random ambient track
	if (rand() % 2) // non zero
	{
		ga.Play("ambientWind", nullptr, true);
	}
	else // zero
	{
		ga.Play("ambientCrows", nullptr, true);

	}
}

void PlayState::OnPause()
{
	GameApp::Get().GetAudio().Pause("ambient");
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

void PlayState::Controls(const GameTimer& gt)
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
	case  ItemCategory::HARVESTED_PLANT:
		if (inventoryPosition->second > 0)
		{
			if (inventoryPosition->first == GC::PLANT_NAME_0)
			{
				mPlayer.health += GC::HEAL_SMALL;
			}
			else if (inventoryPosition->first == GC::PLANT_NAME_1)
			{
				mPlayer.health += GC::HEAL_MED;
			}
			else if (inventoryPosition->first == GC::PLANT_NAME_2)
			{
				mPlayer.health += GC::HEAL_LARGE;
			}
			else
			{
				assert(false); // check ItemCategory for item
			}
			--inventoryPosition->second; //removes a potion

			// Apply health limit
			if (mPlayer.health > GC::PLAYER_MAX_HEALTH)
			{
				mPlayer.health = GC::PLAYER_MAX_HEALTH;
			}

			mPlayerHealthBar.SetValue(mPlayer.health);
		}
		break;
	case ItemCategory::WEAPON:
		if (mCombatController.GetCurrentWeapon() != (*inventoryPosition).first)
		{
			mCombatController.EquipWeapon((*inventoryPosition).first);
		}
		break;
	case ItemCategory::SEED:
		// if there isnt a plant in a radius then instance 
		if (CreatePlant())
		{
			--inventoryPosition->second;
		}

		break;
	case ItemCategory::KEY_ITEM:
		if (inventoryPosition->first == "Radio")
		{
			mMessage.Activate(GC::RADIO_MESSAGES[rand() % GC::RADIO_MESSAGES_SIZE],5.0f);
		}

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

void PlayState::UiUpdate(const GameTimer& gt)
{

	mPlayerHealthBar.Update(gt);

	// animate inventory toggle
	SimpleMath::Vector2 move = SimpleMath::Vector2::Lerp(mInventoryPanel.GetPos(), mInventoryLocation.first, gt.DeltaTime() * 2.0f); // todo Luc move to constant.h
	mInventoryPanel.Move(move, false);
	// padding for text
	move += SimpleMath::Vector2{ 32.0f, 32.0f }; //todo luc move to constants.h
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
	mHelpMessage.Update(gt);
	mTradeHelpMessage.Update(gt);
	mGenerationMessage.Update(gt);
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

void PlayState::UpdatePlayer(const GameTimer& gt)
{
}

void PlayState::UpdatePlants(const GameTimer& gt)
{
	for (auto& p : mPlants)
	{
		p.Update(gt);
	}
}

void PlayState::UpdateTiles(const GameTimer& gt)
{
}

void PlayState::UpdateEnemies(const GameTimer& gt)
{
}

void PlayState::HarvestByRadius()
{
	for (auto& p : mPlants)
	{
		// player within range
		if (SimpleMath::Vector3::Distance(mPlayer.GetPosV3(), p.GetPosV3()) < 1.5f) // todo Luc move to constant.h
		{
			if (p.Harvest()) // applies changes to instance state
			{
				// add all items from lookup to inventory
				for (auto& m : GC::PLANT_LOOKUP_HARVEST.at(p.GetName()))
				{
					mInventory[m.first] += m.second;
				}

				score += GC::SCORE_PLANT;
			}
		}
	}
}

void PlayState::Keyboard(const GameTimer& gt)
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

	//Player attack and harvest
	if (Input::Get().KeyReleased(GC::KEY_ATTACK))
	{
		mCombatController.PlayerAttack();
		HarvestByRadius();
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
	if (Input::Get().KeyReleased(GC::KEY_HELP))
	{
		std::swap(mHelpLocation.first, mHelpLocation.second);
		mHelpMessage.Activate("", 0.f);
	}

	// Trade
	//if (Input::Get().KeyReleased(GC::KEY_TRADE))
	//{
	//	if (FindNearestTraderInRadius())
	//	{
	//		//todo play sound
	//		assert(mpActiveTrader);
	//		GameApp::Get().ChangeState(GC::STATE_TRADE);
	//	}
	//	else
	//	{
	//		//todo sound fail sound
	//	}
	//}
	// Pause
	if (Input::Get().KeyReleased(GC::KEY_PAUSE))
	{
		GameApp::Get().ChangeState("PauseMenu");
	}

	// Debug generate
	if (Input::Get().KeyReleased(GC::KEY_DEBUG_GENERATE)) //todo VOID NEEDS FIXING FIRST
	{
		ReGen();
	}
}

void PlayState::KeyboardDebug(const GameTimer& gt)
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
}

void PlayState::Gamepad(const GameTimer& gt)
{

	// Left stick
	mPlayer.Move(gt, Input::Get().LeftStickXZ() * GC::MOVE_SPEED);

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

void PlayState::Reset()
{
	score = 0;		//Reset Score
	playerName = "";

	mPlayer.Reset();
	mCombatController.Reset();
	mPlayerWeapon.Reset();

	mPlayerHealthBar.SetMinMax(0, GC::PLAYER_MAX_HEALTH); 
	mPlayerHealthBar.SetValue(mPlayer.health);
	mPlayerHealthBar.SetPosition({ 200.0f, 20.0f }); // todo add values to constants.h
	mPlayerHealthBar.SetValue(GC::PLAYER_MAX_HEALTH);

	//Reset Help & Inventory menus
	const DirectX::XMINT2 s = GameApp::Get().GetClientSizeU2(); //todo on resize
	SimpleMath::Vector2 v = GameApp::Get().GetClientSize();

	//Help Menu
	mHelpLocation.second.y = v.y - (v.y / 3);
	mHelpLocation.second.x = 10;
	mHelpLocation.first.y = v.y + 64;
	mHelpLocation.first.x = 10;

	//Inventory Menu
	mInventoryLocation.second.x = v.x - (v.x / 3);
	mInventoryLocation.second.y = 10;

	mInventoryLocation.first.x = v.x + 64;
	mInventoryLocation.first.y = 10;

	//Start Message
	mMessage.mText.center = true;
	mMessage.mText.position = v / 2.0f;
	mMessage.mText.color = DirectX::Colors::Red;
	mMessage.Activate("Start", 1.f);

	//Reset Inventory
	mInventory.clear();
	++mInventory["Radio"];
	++mInventory["Glowing Seeds"];
	++mInventory["Nail Bat"];
	inventoryPosition = mInventory.begin();

	//Reset help text
	mHelpMessage.mText.string = "";
	mTradeHelpMessage.mText.string = "";

	//Reset Traders
	float x = -10.5f;
	const int biggestGap = 4;
	float z = 5.0f;
	for (auto& t : mTraders)
	{
		if (!t.GetIfStoryItem())		//Keeps position of car the same
		{
			x += 1.0f + (rand() % biggestGap);
			z = 5.0f + (rand() % biggestGap);
			t.SetPos({ x, 0.0f, z });
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

	//Reset Lighting
	GameApp::Get().mStoryIndex = 0;
	areas = 0;
	timeCycle = 1;
	timeChange = 0.0f;
	GameApp::Get().GetMainPassCB()->AmbientLight = GC::DAWN_COLOUR; // dawn
	GameApp::Get().GetMainPassCB()->Lights[1].Strength = GC::DAWN_STRENGTH;

	for (auto& e : mAlgorithm.mPopulation)
	{
		e.mEnabled = true;
		e.Reset();

		while (DirectX::SimpleMath::Vector3::Distance(mPlayer.GetPos(), e.GetPos()) < 8.f)	//Prevents enemies from spawning inside a trader
			e.SetRandomPosition();
	}

}

void PlayState::GetName()
{
	ifstream fin("Data/Leaderboard.txt", ios::out);
	if (fin.fail())
		assert(fin.fail());
	else
	{
		std::vector<std::string> lines(6);
		//Get first 6 lines from the txt file
		for (size_t i = 0; i < lines.size(); i++)
			std::getline(fin, lines.at(i));

		playerName = lines.at(5);
		lines.pop_back();

		ofstream fout("Data/Leaderboard.txt", ios::out);
		if (fout.fail())
			assert(fout.fail());
		else
		{
			for (size_t i = 0; i < lines.size(); i++)		//Outputs back into the txt file
				if (i >= 4)
					fout << lines.at(i);
				else
					fout << lines.at(i) << endl;

			fout.close();
		}
	}
}

void PlayState::StoreScore()
{
	//Stores score to txt file
	std::vector<int> scores(5);
	std::vector<std::string> lines(5);

	ifstream fin;
	fin.open("Data/Leaderboard.txt", ios::out);

	if (fin.fail())		//Cannot find the txt file / It doesn't exist
	{
		ofstream fout("Data/Leaderboard.txt");		//Create the file
		if (fout.fail())
			assert(fout.fail());
	}
	else
	{
		//Get first 6 lines from the txt file
		for (size_t i = 0; i < lines.size(); i++)
			std::getline(fin, lines.at(i));

		//Assign player name which is stored on the final line
		lines.push_back(playerName + "." + std::to_string(score));

		//Separate scores from names for each line
		for (size_t i = 0; i < lines.size() - 1; i++)
			scores.at(i) = std::stoi(lines.at(i).substr(lines.at(i).find(".") + 1));

		std::string previousLine;

		ofstream fout("Data/Leaderboard.txt", ios::out);
		if (fout.fail())
			assert(fout.fail());
		else
		{
			for (size_t i = scores.size() - 1; i < -1; i--)		//Reorders score table if positions change
			{
				if (score >= scores.at(i))
				{
					previousLine = lines.at(i);
					lines.at(i) = lines.at(5);
					if (i < 4)
						lines.at(i + 1) = previousLine;
				}
			}

			for (size_t i = 0; i < scores.size(); i++)		//Outputs back into the txt file
				if (i >= 4)
					fout << lines.at(i);
				else
					fout << lines.at(i) << endl;

			fout.close();
		}
	}
}
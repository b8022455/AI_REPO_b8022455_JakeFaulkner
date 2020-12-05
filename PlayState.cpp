#include "PlayState.h"
#include "GameApp.h"

void PlayState::Initialize()
{
	GameApp::Get().SetActiveCamera(&mCamera);

	//mTile.Initialize("Tiles");
	mTileManager.Initialize();

	mCamera.SetPosition(0.0f, 25.0f, 0.0f);

	//GameApp::Get().OnResize();

	mPlayer.Initialize("Player"); // todo adapt GameApp mPlayer to this state
	mPlayerWeapon.Initialize("Weapon");
	


	// Setup temp enemies
	{
		// inserts n of enemies
		mEnemies.push_back(Enemy());
		mEnemies.push_back(Enemy());
		mEnemies.push_back(Enemy());

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

	mCamera.Pitch(XMConvertToRadians(90.0f)); // SETS CAMERA TO FACE DOWN
	
	
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

	std::for_each(mEnemies.begin(), mEnemies.end(), [&](Enemy& e)
	{ 
		e.Update(gt); 
		
		if (mCombatController.CheckCollision(
			e.mpInstance->World._41,
			e.mpInstance->World._42,
			e.mpInstance->World._43 ))
		{
			e.DamageEnemy(5);		//Takes away health from enemy + blowsback enemy position
		}
		
		if (mCombatController.CheckCollision(mPlayer.GetPos(), e.GetPosition()))
		{
			float x = 5.0f;
			mPlayer.DamagePlayer(5);
			mCamera.Strafe(-x * gt.DeltaTime());
			mCamera.UpdateViewMatrix();
		}
	
	});


	PassConstants* pMainPassCB = GameApp::Get().GetMainPassCB();

	//flashing red for low health
	if (mPlayer.health <= 95)
	{
		pMainPassCB->Lights[0].Strength = { sin(gt.TotalTime()) / 2 + 0.5f ,0.0f,0.0f };
	}

	else
	{
		pMainPassCB->Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
	}

	//mCamera.SetPosition(0, 50, 0);

	mCamera.UpdateViewMatrix();

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

		mCamera.Pitch(dy);
		mCamera.RotateY(dx);
	}
}

void PlayState::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void PlayState::OnKeyboardInput(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();

	float moveSpeed = 5.0f;
	float zoomSpeed = 20.0f;

	bool playerMoved = false;

	//input
	if (GetAsyncKeyState(VK_UP/*W*/) & 0x8000) { // Player movement
		 //retool for camera
		mPlayer.MoveUp(gt);
	}

	if (GetAsyncKeyState(VK_DOWN/*S*/) & 0x8000)
	{
		mPlayer.MoveDown(gt);
	}

	if (GetAsyncKeyState(VK_LEFT/*A*/) & 0x8000)
	{
		mPlayer.MoveLeft(gt);
	}

	if (GetAsyncKeyState(VK_RIGHT/*D*/) & 0x8000)
	{
		mPlayer.MoveRight(gt);
	}

	if (GetAsyncKeyState('V') & 0x8000)		///Change key in future
		mCombatController.PlayerAttack();


	// CAMERA MOVEMENT
	if (GetAsyncKeyState(/*VK_UP*/'W') & 0x8000) // implement boundaries
		if (mCamera.GetPosition3f().z <= UPBOUND)
			mCamera.Elevate(moveSpeed * dt);

	if (GetAsyncKeyState(/*VK_DOWN*/'S') & 0x8000)
		if (mCamera.GetPosition3f().z >= DOWNBOUND)
			mCamera.Elevate(-moveSpeed * dt);

	if (GetAsyncKeyState(/*VK_LEFT*/'A') & 0x8000)
		if (mCamera.GetPosition3f().x >= LEFTBOUND)
			mCamera.Strafe(-moveSpeed * dt);

	if (GetAsyncKeyState(/*VK_RIGHT*/'D') & 0x8000)
		if (mCamera.GetPosition3f().x <= RIGHTBOUND)
			mCamera.Strafe(moveSpeed * dt);

	if (GetAsyncKeyState('E') & 0x8000)
		mCamera.Walk(zoomSpeed * dt);

	if (GetAsyncKeyState('Q') & 0x8000)
		mCamera.Walk(-zoomSpeed * dt);

	if (GetAsyncKeyState('P') & 0x8000) // RESETS CAMERA TO ABOVE PLAYER
		mCamera.SetPosition(mPlayer.GetPos().x, mCamera.GetPosition3f().y, mPlayer.GetPos().z);

	// PLAYER MOVEMENT
	if (GetAsyncKeyState(VK_UP/*W*/) & 0x8000) { // Player movement
		 //retool for camera
		mPlayer.MoveUp( gt);
		mCamera.SetPosition(mPlayer.GetPos().x, mCamera.GetPosition3f().y, mPlayer.GetPos().z);
	}

	if (GetAsyncKeyState(VK_DOWN/*S*/) & 0x8000)
	{
		mPlayer.MoveDown( gt);
		mCamera.SetPosition(mPlayer.GetPos().x, mCamera.GetPosition3f().y, mPlayer.GetPos().z);
	}

	if (GetAsyncKeyState(VK_LEFT/*A*/) & 0x8000)
	{
		mPlayer.MoveLeft(gt);
		mCamera.SetPosition(mPlayer.GetPos().x, mCamera.GetPosition3f().y, mPlayer.GetPos().z);
	}

	if (GetAsyncKeyState(VK_RIGHT/*D*/) & 0x8000)
	{
		mPlayer.MoveRight( gt);
		mCamera.SetPosition(mPlayer.GetPos().x, mCamera.GetPosition3f().y, mPlayer.GetPos().z);
	}

	if (GetAsyncKeyState('G') & 0x8000)
	{
	  mEnemies.push_back(Enemy());

	  std::for_each(mEnemies.begin(), mEnemies.end(), [](Enemy& e)
	  {
		e.Initialize("Enemy");

		e.SetRandomPosition();
	  });

	}
}

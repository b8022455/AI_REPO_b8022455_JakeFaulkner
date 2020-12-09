#include "PlayState.h"
#include "GameApp.h"
#include "XmfloatOverload.h"
#include "SimpleMath.h"

void PlayState::Initialize()
{
	GameApp::Get().SetActiveCamera(&mCameras.at(CAMERA_TYPE::GAME));

	//mTile.Initialize("Tiles");
	mTileManager.Initialize();


	for (auto& c : mCameras)
	{
		c.SetPosition(0.0f, 25.0f, 0.0f);
		c.Pitch(XMConvertToRadians(CAM_ANGLE)); // SETS CAMERA TO FACE DOWN
	}

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
			/*mCamera.Strafe(-x * gt.DeltaTime());
			mCamera.UpdateViewMatrix();*/
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

	//mCamera.UpdateViewMatrix();

	for (auto& c : mCameras)
	{
		c.UpdateViewMatrix();
	}

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
	const float dt = gt.DeltaTime();

	float moveSpeed = 5.0f;
	float zoomSpeed = 20.0f;

	bool playerMoved = false;

	// Player movement
	if (GetAsyncKeyState(VK_UP/*W*/) & 0x8000) 
	{
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

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		mCombatController.PlayerAttack();

	// Switch to debug camera
	if (GetAsyncKeyState('9') & 0x8000)
	{
		mCamType = CAMERA_TYPE::DEBUG;
		//Set debug cam to game position to avoid getting lost
		mCameras.at(CAMERA_TYPE::DEBUG).SetPosition(mCameras.at(CAMERA_TYPE::GAME).GetPosition3f());

		//Update active camera
		GameApp::Get().SetActiveCamera(&mCameras.at(CAMERA_TYPE::DEBUG));
	}

	// Switch to game camera
	if (GetAsyncKeyState('8') & 0x8000)
	{
		mCamType = CAMERA_TYPE::GAME;
		//Update active camera
		GameApp::Get().SetActiveCamera(&mCameras.at(CAMERA_TYPE::GAME));
	}

	// CAMERA MOVEMENT FOR DEBUG
	if (mCamType == CAMERA_TYPE::DEBUG)
	{
		if (GetAsyncKeyState('E') & 0x8000) // implement boundaries
			//if (mCameraDebug.GetPosition3f().z <= UPBOUND)
			mCameras.at(CAMERA_TYPE::DEBUG).Elevate(moveSpeed * dt);

		if (GetAsyncKeyState('Q') & 0x8000)
			//if (mCameraDebug.GetPosition3f().z >= DOWNBOUND)
			mCameras.at(CAMERA_TYPE::DEBUG).Elevate(-moveSpeed * dt);

		if (GetAsyncKeyState('A') & 0x8000)
			//if (mCameraDebug.GetPosition3f().x >= LEFTBOUND)
			mCameras.at(CAMERA_TYPE::DEBUG).Strafe(-moveSpeed * dt);

		if (GetAsyncKeyState('D') & 0x8000)
			//if (mCameraDebug.GetPosition3f().x <= RIGHTBOUND)
			mCameras.at(CAMERA_TYPE::DEBUG).Strafe(moveSpeed * dt);

		if (GetAsyncKeyState('W') & 0x8000)
			mCameras.at(CAMERA_TYPE::DEBUG).Walk(zoomSpeed * dt);

		if (GetAsyncKeyState('S') & 0x8000)
			mCameras.at(CAMERA_TYPE::DEBUG).Walk(-zoomSpeed * dt);

		if (GetAsyncKeyState('P') & 0x8000) // RESETS CAMERA TO ABOVE PLAYER
			mCameras.at(CAMERA_TYPE::DEBUG).SetPosition(mPlayer.GetPos());

	}

	// PLAYER MOVEMENT
	if (GetAsyncKeyState(VK_UP/*W*/) & 0x8000) { // Player movement
		 //retool for camera
		mPlayer.MoveUp( gt);
	}

	if (GetAsyncKeyState(VK_DOWN/*S*/) & 0x8000)
	{
		mPlayer.MoveDown( gt);
	}

	if (GetAsyncKeyState(VK_LEFT/*A*/) & 0x8000)
	{
		mPlayer.MoveLeft(gt);
	}

	if (GetAsyncKeyState(VK_RIGHT/*D*/) & 0x8000)
	{
		mPlayer.MoveRight( gt);
	}

	if (GetAsyncKeyState('G') & 0x8000)
	{
	  mEnemies.push_back(Enemy());

	  std::for_each(mEnemies.begin(), mEnemies.end(), [](Enemy& e)
	  {
		e.SetRandomPosition();
	  });

	}

	Inventory.push_back({ "Potion" });
	Inventory.push_back({ "Berry" });
}

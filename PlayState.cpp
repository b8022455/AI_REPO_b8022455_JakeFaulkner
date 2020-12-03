#include "PlayState.h"
#include "GameApp.h"

void PlayState::Initialize()
{
	mPlayer.Initialize("Player"); // todo adapt GameApp mPlayer to this state
	mGameObject.Initialize("Weapon");
	mCombatController.Initialize();

	mEnemy.Initialize("Enemy");

	mCamera.Pitch(XMConvertToRadians(90.0f)); // SETS CAMERA TO FACE DOWN
	
	GameApp::Get().SetActiveCamera(&mCamera);
	
}

void PlayState::Update(const GameTimer & gt)
{

	mGameObject.Update(gt);
	mCombatController.Update(mAllRitems);
	mEnemy.Update(gt);


	//Checks if weapon is colliding w/ example box
	if (mCombatController.CheckCollision(mAllRitems["Enemy"]->Instances.at(0).World._41, mAllRitems["Enemy"]->Instances.at(0).World._42,
		mAllRitems["Enemy"]->Instances.at(0).World._43))
	{
		mAllRitems["Enemy"]->Instances.at(0).MaterialIndex = 5;			//Visual representation for collision
		
		enemyHealth -= 5;
		mAllRitems["Enemy"]->Instances.at(0).World._41 += 5.0f;			///Pushes enemy back after being hit by sword, In future have enemy move back based on which way player is facing !!!
	}

	///Enemy Pos, Remove into Enemy class in future!!!
	XMFLOAT3 enemyPos = XMFLOAT3(mAllRitems["Enemy"]->Instances.at(0).World._41, mAllRitems["Enemy"]->Instances.at(0).World._42, mAllRitems["Enemy"]->Instances.at(0).World._43);

	//Interaction stuff
	if (mCombatController.CheckCollision(mPlayer.GetPos(), enemyPos))			//Checks the distance between the player and the enemy objects
	{
		mPlayer.health -= 5;
		mAllRitems["Player"]->Instances.at(0).World._41 -= 5.0f;		///Find way to connect this to player class !!!

		mCamera.Strafe(-5.0f * gt.DeltaTime());
		mCamera.UpdateViewMatrix();
	}

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
		mCombatController.PlayerAttack(mAllRitems);


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
}

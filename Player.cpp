#include "Player.h"
#include "GameApp.h"

//#include "XmfloatOverload.h"

void Player::Update(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();	
	GameApp::Get().mDebugLog << "Health:" << health << "\n";
	// APPLY VELOCITY HERE
	if (RIGHT_velocity >= 0)
		RIGHT_velocity -= dt * 8;
	if (RIGHT_velocity < 0)
		RIGHT_velocity = 0;
	if (LEFT_velocity >= 0)
		LEFT_velocity -= dt * 8;
	if (LEFT_velocity < 0)
		LEFT_velocity = 0;
	if (UP_velocity >= 0)
		UP_velocity -= dt * 8;
	if (UP_velocity < 0)
		UP_velocity = 0;
	if (DOWN_velocity >= 0)
		DOWN_velocity -= dt * 8;
	if (DOWN_velocity < 0)
		DOWN_velocity = 0;
	
	if (mpInstance->World._41 <= PLAYER_RIGHTBOUND) {
		if (Slowed == true) 
			mpInstance->World._41 += (0.5f * RIGHT_velocity) * dt;
		if (Slippy == true) 
			mpInstance->World._41 += (1.5f * RIGHT_velocity) * dt;
		if (Slowed != true && Slippy != true) 
			mpInstance->World._41 += RIGHT_velocity * dt;
	}
	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 -= (0.5f * LEFT_velocity) * dt;
		if (Slippy == true)
			mpInstance->World._41 -= (1.5f * LEFT_velocity) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._41 -= LEFT_velocity * dt;
	}
	if (mpInstance->World._43 <= PLAYER_UPBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 += (0.5f * UP_velocity) * dt;
		if (Slippy == true)
			mpInstance->World._43 += (1.5f * UP_velocity) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._43 += UP_velocity * dt;
	}
	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 -= (0.5f * DOWN_velocity) * dt;
		if (Slippy == true)
			mpInstance->World._43 -= (1.5f * DOWN_velocity) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._43 -= DOWN_velocity * dt;
	}

	// if moving count down to next step
	if (RIGHT_velocity > 0.1f || LEFT_velocity > 0.1f || UP_velocity > 0.1f || DOWN_velocity > 0.1f)
	{
		if (mFootstepTimer.HasTimeElapsed(gt.DeltaTime(), 0.5f))
		{
			GameApp::Get().GetAudio().Play("playerFootstep", nullptr, false, 1.0f, GC::FOOTSTEP_PITCH[rand() % GC::FOOTSTEP_PITCH_COUNT]);
		}
	}
}

void Player::MoveUp( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

  // INCREASE VELOCITY HERE
  if (UP_velocity < MAX_VELOCITY)
	  UP_velocity += dt * 12; // add double time to velocity

  /*if (mpInstance->World._43 <= PLAYER_UPBOUND)
  {
	  if (Slowed == true) 
		mpInstance->World._43 += (0.5f * MOVE) * dt;
	  if (Slowed == false)
		  mpInstance->World._43 += MOVE * dt;
  }*/
  playerDir = PlayerFacingDirection::Up;
}

void Player::MoveDown(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	// INCREASE VELOCITY HERE
	if (DOWN_velocity < MAX_VELOCITY)
		DOWN_velocity += dt * 12; // add double time to velocity

	/*if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 -= (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._43 -= MOVE * dt;
	}*/
	playerDir = PlayerFacingDirection::Down;
}

void Player::MoveLeft(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	// INCREASE VELOCITY HERE
	if (LEFT_velocity < MAX_VELOCITY)
		LEFT_velocity += dt * 12; // add double time to velocity

	/*if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 -= (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._41 -= MOVE * dt;
		}*/
	playerDir = PlayerFacingDirection::Left;

}

void Player::MoveRight(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	// APPLY VELOCITY EVERY FRAME IN UPDATE
	// INCREASE VELOCITY HERE
	if (RIGHT_velocity < MAX_VELOCITY)
		RIGHT_velocity += dt * 12; // add double time to velocity

	//if (mpInstance->World._41 <= PLAYER_RIGHTBOUND)
	//{
	//	if (Slowed == true)
	//		mpInstance->World._41 += (0.5f * MOVE) * dt;
	//	if (Slowed == false)
	//		mpInstance->World._41 += MOVE * dt;
	//}
	playerDir = PlayerFacingDirection::Right;
}



void Player::Move(const GameTimer & gt, const DirectX::SimpleMath::Vector3 & vec)
{
	//Changes position of sword depending on where player is facing
	if (vec.x < 0)
		playerDir = PlayerFacingDirection::Left;
	else if (vec.x > 0)
		playerDir = PlayerFacingDirection::Right;

	if (vec.z > 0)
		playerDir = PlayerFacingDirection::Up;
	else if (vec.z < 0)
		playerDir = PlayerFacingDirection::Down;

	//todo simplify

	vel.SetVel(vec, 1.0f*gt.DeltaTime());
	SimpleMath::Vector3 pos = GetPos();
	SimpleMath::Vector3 v = vel.GetVelocity();

	SetPos(pos + v);
}

void Player::DamagePlayer(int damage)			//When enemy hits with player
{
	float x = 0.0f;
	float z = 0.0f;
	health -= damage;

	switch (playerDir)
	{
	case 0:									//Left
		x = 5.0f;
		break;

	case 1:									//Right
		x = -5.0f;
		break;

	case 2:									//Up
		z = -5.0f;
		break;

	case 3:									//Down
		z = 5.0f;
		break;
	}

	mpInstance->World._41 += x;
	mpInstance->World._43 += z;
}

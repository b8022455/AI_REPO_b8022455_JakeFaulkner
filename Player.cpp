#include "Player.h"
#include "GameApp.h"



void Player::Update(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();	
	GameApp::Get().mDebugLog << "Health:" << health << "\n";
	// TODO: APPLY VELOCITY HERE
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
		if (Slowed == true) {
			mpInstance->World._41 += (0.5f * RIGHT_velocity) * dt;
		}
		if (Slowed != true) {
			mpInstance->World._41 += RIGHT_velocity * dt;
		}
	}
	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 -= (0.5f * LEFT_velocity) * dt;
		if (Slowed == false)
			mpInstance->World._41 -= LEFT_velocity * dt;
	}
	if (mpInstance->World._43 <= PLAYER_UPBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 += (0.5f * UP_velocity) * dt;
		if (Slowed == false)
			mpInstance->World._43 += UP_velocity * dt;
	}
	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 -= (0.5f * DOWN_velocity) * dt;
		if (Slowed == false)
			mpInstance->World._43 -= DOWN_velocity * dt;
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
}

void Player::MoveDown( const GameTimer& gt)
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

}

void Player::MoveLeft( const GameTimer& gt)
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

}

void Player::MoveRight( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	// TODO: velocity test here
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

}



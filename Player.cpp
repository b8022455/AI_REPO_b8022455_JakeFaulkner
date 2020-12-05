#include "Player.h"
#include "GameApp.h"



void Player::Update(const GameTimer & gt)
{
	GameApp::Get().mDebugLog << "Health:" << health << "\n";
}

void Player::MoveUp( const GameTimer& gt)
{
  const float dt = gt.DeltaTime();

  if (mpInstance->World._43 <= PLAYER_UPBOUND)
  {
	  if (Slowed == true) 
		mpInstance->World._43 += (0.5f * MOVE) * dt;
	  if (Slowed == false)
		  mpInstance->World._43 += MOVE * dt;
  }
}

void Player::MoveDown( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 -= (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._43 -= MOVE * dt;
	}

}

void Player::MoveLeft( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 -= (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._41 -= MOVE * dt;
	}

}

void Player::MoveRight( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (mpInstance->World._41 <= PLAYER_RIGHTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 += (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._41 += MOVE * dt;
	}

}



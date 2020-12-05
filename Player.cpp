#include "Player.h"
#include "GameApp.h"



void Player::Update(const GameTimer & gt)
{
	GameApp::Get().mDebugLog << "Health:" << health << "\n";
}

void Player::MoveUp( const GameTimer& gt)
{
  const float dt = gt.DeltaTime();
  //const float moveSpeed = 5.0f;

  if (mpInstance->World._43 <= PLAYER_UPBOUND)
  {
	  mpInstance->World._43 += moveSpeed * dt;
  }
}

void Player::MoveDown( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	//const float moveSpeed = 5.0f;

	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		mpInstance->World._43 -= moveSpeed * dt;
	}

}

void Player::MoveLeft( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	//const float moveSpeed = 5.0f;

	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		mpInstance->World._41 -= moveSpeed * dt;
	}

}

void Player::MoveRight( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	//const float moveSpeed = 5.0f;

	if (mpInstance->World._41 <= PLAYER_RIGHTBOUND)
	{
		mpInstance->World._41 += moveSpeed * dt;
	}

}



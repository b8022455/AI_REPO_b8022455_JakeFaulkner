#include "Player.h"


void Player::UpdatePos()
{
  //float playerXPos = mAllRitems["Player"]->Instances.at(0).World._41;
  //float playerYPos = mAllRitems["Player"]->Instances.at(0).World._42;
  //float playerZPos = mAllRitems["Player"]->Instances.at(0).World._43;
  //
  //XMFLOAT3 playerPos = XMFLOAT3(playerXPos, playerYPos, playerZPos); // todo: Whats this? It's an unused temp variable. Should this be pos?
}

//todo remove map param
XMFLOAT3 Player::GetPos()
{

  return { 
	  mpInstance->World._41 , 
	  mpInstance->World._42 ,
	  mpInstance->World._43 
  };
}


void Player::MoveUp( const GameTimer& gt) 
{
  const float dt = gt.DeltaTime();
  const float moveSpeed = 5.0f;

  if (mpInstance->World._43 <= PLAYER_UPBOUND)
  {
	  mpInstance->World._43 += moveSpeed * dt;
  }
}

void Player::MoveDown( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		mpInstance->World._43 -= moveSpeed * dt;
	}

}

void Player::MoveLeft( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		mpInstance->World._41 -= moveSpeed * dt;
	}

}

void Player::MoveRight( const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	if (mpInstance->World._41 <= PLAYER_RIGHTBOUND)
	{
		mpInstance->World._41 += moveSpeed * dt;
	}

}



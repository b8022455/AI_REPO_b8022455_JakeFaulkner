#include "Player.h"


void Player::UpdatePos(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
  float playerXPos = mAllRitems["Player"]->Instances.at(0).World._41;
  float playerYPos = mAllRitems["Player"]->Instances.at(0).World._42;
  float playerZPos = mAllRitems["Player"]->Instances.at(0).World._43;

  XMFLOAT3 playerPos = XMFLOAT3(playerXPos, playerYPos, playerZPos); // todo: Whats this? It's an unused temp variable. Should this be pos?
}

//todo remove map param
XMFLOAT3 Player::GetPos(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{

  return { 
	  mpInstance->World._41 , 
	  mpInstance->World._42 ,
	  mpInstance->World._43 
  };
}


void Player::MoveUp(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems, const GameTimer& gt) 
{
  const float dt = gt.DeltaTime();
  const float moveSpeed = 5.0f;

  if (mpInstance->World._43 <= PLAYER_UPBOUND)
  {
	  mpInstance->World._43 += moveSpeed * dt;
  }
}

void Player::MoveDown(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		mpInstance->World._43 -= moveSpeed * dt;
	}

}

void Player::MoveLeft(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		mpInstance->World._41 -= moveSpeed * dt;
	}

}

void Player::MoveRight(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	if (mpInstance->World._41 <= PLAYER_RIGHTBOUND)
	{
		mpInstance->World._41 += moveSpeed * dt;
	}

	//XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World); // ORIGINAL MOVEMENT
	//XMFLOAT4X4 test;
	//XMStoreFloat4x4(&test, current);

	//if (test._41 <= PLAYER_RIGHTBOUND) {
	//	XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(moveSpeed * dt, 0.0f, 0.0f));
	//	transform = XMMatrixMultiply(current, transform);
	//	XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
	//}
}



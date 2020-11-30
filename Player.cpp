#include "Player.h"

void Player::Initialize()
{
  health = 100;
}

void Player::UpdatePos(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
  float playerXPos = mAllRitems["Player"]->Instances.at(0).World._41;
  float playerYPos = mAllRitems["Player"]->Instances.at(0).World._42;
  float playerZPos = mAllRitems["Player"]->Instances.at(0).World._43;

  XMFLOAT3 playerPos = XMFLOAT3(playerXPos, playerYPos, playerZPos);
}

XMFLOAT3 Player::GetPos(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
  float playerXPos = mAllRitems["Player"]->Instances.at(0).World._41;
  float playerYPos = mAllRitems["Player"]->Instances.at(0).World._42;
  float playerZPos = mAllRitems["Player"]->Instances.at(0).World._43;

  XMFLOAT3 playerPos = XMFLOAT3(playerXPos, playerYPos, playerZPos);

  return playerPos;
}


void Player::MoveUp(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems, const GameTimer& gt) 
{
  const float dt = gt.DeltaTime();
  const float moveSpeed = 5.0f;

  XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World); // ORIGINAL MOVEMENT
  XMFLOAT4X4 test;
  XMStoreFloat4x4(&test, current);
  if (test._43 <= PLAYER_UPBOUND) {
	  XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, 0.0f, moveSpeed * dt));
	  transform = XMMatrixMultiply(current, transform);
	  XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
  }
  // cant move diagonally, 2 key presses at once just does first key
  //TODO: make camera follow player
}

void Player::MoveDown(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World); // ORIGINAL MOVEMENT
	XMFLOAT4X4 test;
	XMStoreFloat4x4(&test, current);

	if (test._43 >= PLAYER_DOWNBOUND) {
			XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, 0.0f, -moveSpeed * dt));
			transform = XMMatrixMultiply(current, transform);
			XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
	}
}

void Player::MoveLeft(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World); // ORIGINAL MOVEMENT
	XMFLOAT4X4 test;
	XMStoreFloat4x4(&test, current);

	if (test._41 >= PLAYER_LEFTBOUND) {
		XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(-moveSpeed * dt, 0.0f, 0.0f));
		transform = XMMatrixMultiply(current, transform);
		XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
	}
}

void Player::MoveRight(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float moveSpeed = 5.0f;

	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World); // ORIGINAL MOVEMENT
	XMFLOAT4X4 test;
	XMStoreFloat4x4(&test, current);

	if (test._41 <= PLAYER_RIGHTBOUND) {
		XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(moveSpeed * dt, 0.0f, 0.0f));
		transform = XMMatrixMultiply(current, transform);
		XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
	}
}



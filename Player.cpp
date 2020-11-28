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


void Player::Move(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems, const GameTimer& gt)
{
  const float dt = gt.DeltaTime();

  // cant move diagonally, 2 key presses at once just does first key
  //TODO: make camera follow player
  if (GetAsyncKeyState(VK_LEFT) & 0x8000)
  {
	// TODO: WORKING MOVEMENT TEST LOCATED HERE
	XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(-5.0f * dt, 0.0f, 0.0f));
	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World);
	transform = XMMatrixMultiply(current, transform);
	XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
  }

  if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
  {
	// TODO: WORKING MOVEMENT TEST LOCATED HERE
	XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(5.0f * dt, 0.0f, 0.0f));
	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World);
	transform = XMMatrixMultiply(current, transform);
	XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
  }


  if (GetAsyncKeyState(VK_UP) & 0x8000)
  {
	// TODO: WORKING MOVEMENT TEST LOCATED HERE
	XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, 0.0f, 5.0f * dt));
	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World);
	transform = XMMatrixMultiply(current, transform);
	XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
  }

  if (GetAsyncKeyState(VK_DOWN) & 0x8000)
  {
	// TODO: WORKING MOVEMENT TEST LOCATED HERE
	XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, 0.0f, -5.0f * dt));
	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World);
	transform = XMMatrixMultiply(current, transform);
	XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
  }


}
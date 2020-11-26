#include "Player.h"

void Player::Initialize()
{
  health = 100;
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
	mCamera.SetPosition(-5.0f * dt, 50.0f, 0.0f);
  }

  else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
  {
	// TODO: WORKING MOVEMENT TEST LOCATED HERE
	XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(5.0f * dt, 0.0f, 0.0f));
	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World);
	transform = XMMatrixMultiply(current, transform);
	XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
	mCamera.SetPosition(5.0f * dt, 50.0f, 0.0f);
  }


  else if (GetAsyncKeyState(VK_UP) & 0x8000)
  {
	// TODO: WORKING MOVEMENT TEST LOCATED HERE
	XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, 0.0f, 5.0f * dt));
	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World);
	transform = XMMatrixMultiply(current, transform);
	XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
	mCamera.SetPosition(0.0f, 50.0f, 5.0f * dt);
  }

  else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
  {
	// TODO: WORKING MOVEMENT TEST LOCATED HERE
	XMMATRIX transform = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslation(0.0f, 0.0f, -5.0f * dt));
	XMMATRIX current = XMLoadFloat4x4(&mAllRitems["Player"]->Instances.at(0).World);
	transform = XMMatrixMultiply(current, transform);
	XMStoreFloat4x4(&mAllRitems["Player"]->Instances.at(0).World, transform);
	mCamera.SetPosition(0.0f, 50.0f, -5.0f * dt);
  }


}
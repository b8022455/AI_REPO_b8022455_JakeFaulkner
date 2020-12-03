#pragma once

#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "Common/Camera.h"
#include "GameObject.h"


using namespace DirectX;

class Player : public GameObject
{
public:
  void MoveUp		( const GameTimer& gt);
  void MoveDown		( const GameTimer& gt);
  void MoveLeft		( const GameTimer& gt);
  void MoveRight	( const GameTimer& gt);
  void UpdatePos	();
  XMFLOAT3 GetPos();
  int health = 100;

  const float PLAYER_RIGHTBOUND = 15.0f;
  const float PLAYER_LEFTBOUND = -15.0f;

  const float PLAYER_UPBOUND = 15.0f;
  const float PLAYER_DOWNBOUND = -15.0f;

private:
  void DamageEnemy();		
  void DamagePlayer();


  XMFLOAT3 pos;
  

 
};
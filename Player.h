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

	void Update(const GameTimer& gt);
  void MoveUp		( const GameTimer& gt);
  void MoveDown		( const GameTimer& gt);
  void MoveLeft		( const GameTimer& gt);
  void MoveRight	( const GameTimer& gt);

  int health = 100;

  float hazardTimer = 0; // needs to be set as needed, used to apply hazard effects (not every frame)
  bool Slowed = false;

  // used to apply boundaries to player
  const float PLAYER_RIGHTBOUND = 15.0f;
  const float PLAYER_LEFTBOUND = -15.0f;

  const float PLAYER_UPBOUND = 15.0f;
  const float PLAYER_DOWNBOUND = -15.0f;

private:
  void DamageEnemy();		
  void DamagePlayer();


  XMFLOAT3 pos;
  
  const float MOVE = 5.0f;
 
};
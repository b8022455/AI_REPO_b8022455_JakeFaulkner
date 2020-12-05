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

  void DamagePlayer(int damage);

  int health = 100;

  float hazardTimer = 0; // needs to be set as needed, used to apply hazard effects (not every frame)
  bool Slowed = false;

  // used to apply boundaries to player
  const float PLAYER_RIGHTBOUND = 15.0f;
  const float PLAYER_LEFTBOUND = -15.0f;

  const float PLAYER_UPBOUND = 15.0f;
  const float PLAYER_DOWNBOUND = -15.0f;

  bool facingLeft = false;
  bool facingRight = true;

private:
  void DamageEnemy();		

  XMFLOAT3 pos;
  
  const float MOVE = 5.0f;
  const float MAX_VELOCITY = 5.0f; // change as needed, replace move
  const float START_VELOCITY = 1.0f; // may not be needed
  // velocity increases with time as long as the button is held, velocity decreases to nothing when button not held
  // constantly apply velocity, but change when button is pressed
  float UP_velocity = 0.0f; //  increases when moving upwards up to max, decreases when not moving
  float DOWN_velocity = 0.0f; //  increases when moving downwards up to max, decreases when not moving
  float LEFT_velocity = 0.0f; //  increases when moving leftwards up to max, decreases when not moving
  float RIGHT_velocity = 0.0f; //  increases when moving rightwards up to max, decreases when not moving

};
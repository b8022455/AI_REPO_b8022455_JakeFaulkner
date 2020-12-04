#pragma once

#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "Common/Camera.h"

using namespace DirectX;

class Player
{
public:
  Player() {};	//Default Constructor

  void Initialize();
  void MoveUp(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems, const GameTimer& gt);
  void MoveDown(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt);
  void MoveLeft(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt);
  void MoveRight(std::unordered_map<std::string, std::unique_ptr<RenderItem>>& mAllRitems, const GameTimer& gt);
  void UpdatePos(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems);
  XMFLOAT3 GetPos(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems);
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
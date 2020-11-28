#pragma once

#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "RenderItem.h"


using namespace DirectX;

class Player
{
public:
  Player() {};	//Default Constructor

  void Initialize();
  void Move(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems, const GameTimer& gt);
  void UpdatePos(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems);
  XMFLOAT3 GetPos(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems);



private:
  void DamageEnemy();		
  void DamagePlayer();

  XMFLOAT3 pos;
  int health = 100;

 
};
#pragma once

#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "Common/Camera.h"
#include "FrameResource.h"
#include "RenderItem.h"


using namespace DirectX;

class Player
{
public:
  Player() {};	//Default Constructor

  void Initialize();
  void Move(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems, const GameTimer& gt);


private:
  void DamageEnemy();		
  void DamagePlayer();

  Camera mCamera;

  int health = 100;

 
};
#pragma once

#include "Common/d3dUtil.h"
#include "FrameResource.h"
#include <memory>

class GameTimer;

class GameObject
{
	
	DirectX::XMFLOAT4X4 world;
	std::unique_ptr<InstanceData> instance;// todo how to pass in. Make GameApp/D3DApp singleton?
	
	//
	DirectX::XMFLOAT3 direction;
	float speed;

public:
	virtual void Init() = 0;
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw() = 0;

};


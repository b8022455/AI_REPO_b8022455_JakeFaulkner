#pragma once

#include "Common/d3dUtil.h"
#include "FrameResource.h"
#include <memory>
#include <DirectXMath.h>

class GameTimer;

class GameObject
{
protected:
	// Adds and points to an existing render item
	void AddRenderItemInstance(const std::string& renderItemName);
	void RemoveRenderItemInstance();
public:
	//todo make protected
	InstanceData* mpInstance;
	// Pass in renderItem string key and Initialize adds and points to an existing render item. Call AddRenderItemInstance() if overriding
	virtual void Initialize(const std::string& renderItemName);
	virtual void Update(const GameTimer& gt) {};

	void SetPos(const DirectX::XMFLOAT3& newPos);
	DirectX::XMFLOAT3 GetPos();

};
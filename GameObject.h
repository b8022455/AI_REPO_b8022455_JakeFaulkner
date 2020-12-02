#pragma once

#include "Common/d3dUtil.h"
#include "FrameResource.h"
#include <memory>

class GameTimer;

class GameObject
{
protected:
	InstanceData* mpInstance;
	// Adds and points to an existing render item
	void AddRenderItemInstance(const std::string& renderItemName);
public:
	// Pass in renderItem string key and Initialize adds and points to an existing render item. Call AddRenderItemInstance() if overriding
	virtual void Initialize(const std::string& renderItemName);
	virtual void Update(const GameTimer& gt) {};


};
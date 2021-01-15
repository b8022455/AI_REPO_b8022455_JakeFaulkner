#pragma once

#include "Common/d3dUtil.h"
#include "FrameResource.h"
#include <memory>
#include <DirectXMath.h>
#include "SimpleMath.h"


class GameTimer;

//Initialize needs to be called for every gameobject with render item name
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
	const DirectX::XMFLOAT3 GetPos() const;
	const DirectX::SimpleMath::Vector3 GetPosV3() const;

	void SetScale(const float& scale);
	void SetScaleX(const float scaleX);
	void SetScaleY(const float scaleY);
	void SetScaleZ(const float scaleZ);

	void SetRotationX(const float newRotation);
	void SetRotationY(const float newRotation);
	void SetRotationZ(const float newRotation);
	void SetRotation(const float rotationX, const float rotationY, const float rotationZ);

	const void RotationAroundPoint(const DirectX::XMFLOAT3 translation1, const float rotation, const DirectX::XMFLOAT3 translation2) const;
	const void RotationAroundPoint(const DirectX::XMMATRIX translation1, const float rotation, const DirectX::XMFLOAT3 translation2) const;

	bool CheckCollision(const DirectX::XMFLOAT3 Object1, const DirectX::XMFLOAT3 Object2);
};
#include "GameObject.h"
#include "GameApp.h"
#include <DirectXMath.h>

void GameObject::AddRenderItemInstance(const std::string & renderItemName)
{
	mpInstance = GameApp::Get().AddRenderItemInstance(renderItemName);
	assert(mpInstance);
}

void GameObject::RemoveRenderItemInstance()
{
	// todo May be an issue with pointer mpInstance for moved instances that remain
}

void GameObject::Initialize(const std::string & renderItemName)
{
	AddRenderItemInstance(renderItemName);
}

void GameObject::SetPos(const DirectX::XMFLOAT3 & newPos)
{
	mpInstance->World._41 = newPos.x;
	mpInstance->World._42 = newPos.y;
	mpInstance->World._43 = newPos.z;
}

const DirectX::XMFLOAT3 GameObject::GetPos() const
{
	return DirectX::XMFLOAT3
	{
		mpInstance->World._41,
		mpInstance->World._42,
		mpInstance->World._43
	};
}

const DirectX::SimpleMath::Vector3 GameObject::GetPosV3() const
{
	return DirectX::XMFLOAT3
	{
		mpInstance->World._41,
		mpInstance->World._42,
		mpInstance->World._43
	};
}

void GameObject::SetScale(const float& scale)
{
	mpInstance->World._11 = scale;
	mpInstance->World._22 = scale;
	mpInstance->World._33 = scale;
}

void GameObject::SetScaleX(const float scale)
{
	mpInstance->World._11 = scale;
}

void GameObject::SetScaleY(const float scale)
{
	mpInstance->World._22 = scale;
}

void GameObject::SetScaleZ(const float scale)
{
	mpInstance->World._33 = scale;
}

void GameObject::SetRotationX(const float newRotation)
{
	float convertToRad = newRotation * 3.14159f / 180.0f;
	float cosineAngle = cos(convertToRad);
	float sinAngle = sin(convertToRad);

	this->mpInstance->World._22 = cosineAngle;
	this->mpInstance->World._23 = -sinAngle;
	this->mpInstance->World._32 = sinAngle;
	this->mpInstance->World._33 = cosineAngle;
}

void GameObject::SetRotationY(const float newRotation)
{
	float convertToRad = newRotation * 3.14159f / 180.0f;
	float cosineAngle = cos(convertToRad);
	float sinAngle = sin(convertToRad);

	this->mpInstance->World._11 = cosineAngle;
	this->mpInstance->World._13 = sinAngle;
	this->mpInstance->World._31 = -sinAngle;
	this->mpInstance->World._33 = cosineAngle;
}

void GameObject::SetRotationZ(const float newRotation)
{
	float convertToRad = newRotation * 3.14159f / 180.0f;
	float cosineAngle = cos(convertToRad);
	float sinAngle = sin(convertToRad);

	this->mpInstance->World._11 = cosineAngle;
	this->mpInstance->World._12 = -sinAngle;
	this->mpInstance->World._21 = sinAngle;
	this->mpInstance->World._22 = cosineAngle;
}

void GameObject::SetRotation(const float x, const float y, const float z)
{
	//X
	float degreesX = x * 3.14159f / 180.0f;
	float cosineX = cos(degreesX);
	float sinX = sin(degreesX);

	this->mpInstance->World._22 = cosineX;
	this->mpInstance->World._23 = -sinX;
	this->mpInstance->World._32 = sinX;
	this->mpInstance->World._33 = cosineX;

	//Y
	float degreesY = y * 3.14159f / 180.0f;
	float cosineY = cos(degreesY);
	float sinY = sin(degreesY);

	this->mpInstance->World._11 = cosineY;
	this->mpInstance->World._13 = sinY;
	this->mpInstance->World._31 = -sinY;
	this->mpInstance->World._33 = cosineY;

	//Z
	float degreesZ = z * 3.14159f / 180.0f;
	float cosineZ = cos(degreesZ);
	float sinZ = sin(degreesZ);

	this->mpInstance->World._11 = cosineZ;
	this->mpInstance->World._12 = -sinZ;
	this->mpInstance->World._21 = sinZ;
	this->mpInstance->World._22 = cosineZ;
}

const void GameObject::RotationAroundPoint(const DirectX::XMFLOAT3 translate1, const float rotation, const DirectX::XMFLOAT3 translate2) const
{
	float convertToRad = rotation * 3.14159f / 180.0f;
	DirectX::XMMATRIX transformation;

	transformation = XMMatrixTranslation(translate1.x, translate1.y, translate1.z);
	transformation *= XMMatrixRotationY(convertToRad);
	transformation *= XMMatrixTranslation(translate2.x, translate2.y, translate2.z);

	XMStoreFloat4x4(&mpInstance->World, transformation);
}

const void GameObject::RotationAroundPoint(const DirectX::XMMATRIX translate1, const float rotation, const DirectX::XMFLOAT3 translate2) const
{
	float convertToRad = rotation * 3.14159f / 180.0f;
	XMMATRIX transformation = translate1;		//First Translation already done and passed via the transformation parameter

	transformation *= XMMatrixRotationY(convertToRad);
	transformation *= XMMatrixTranslation(translate2.x, translate2.y, translate2.z);

	XMStoreFloat4x4(&mpInstance->World, transformation);
}

bool GameObject::CheckCollision(XMFLOAT3 Object1, XMFLOAT3 Object2)
{
	//Could change to have Mathf.Abs() to only check the positive boundary?
	float distanceInX = Object1.x - Object2.x;
	float distanceInY = Object1.y - Object2.y;
	float distanceInZ = Object1.z - Object2.z;

	if (distanceInX > -1.0f && distanceInX < 1.0f)
		if (distanceInY > -1.5f && distanceInY < 1.5f)
			if (distanceInZ > -1.0f && distanceInZ < 1.0f)
				return true;

	return false;
}
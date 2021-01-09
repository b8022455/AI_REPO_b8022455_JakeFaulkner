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

void GameObject::SetRotationX(const float newRotation)
{
	float convertToRad = newRotation * 3.14159f / 180.0f;
	XMMATRIX rotation = XMMatrixRotationX(convertToRad);
	rotation *= XMMatrixTranslation(mpInstance->World._41, mpInstance->World._42, mpInstance->World._43);
	XMStoreFloat4x4(&mpInstance->World, rotation);
}

void GameObject::SetRotationY(const float newRotation)
{
	float convertToRad = newRotation * 3.14159f / 180.0f;
	XMMATRIX rotation = XMMatrixRotationY(convertToRad);
	rotation *= XMMatrixTranslation(mpInstance->World._41, mpInstance->World._42, mpInstance->World._43);
	XMStoreFloat4x4(&mpInstance->World, rotation);
}

void GameObject::SetRotationZ(const float newRotation)
{
	float convertToRad = newRotation * 3.14159f / 180.0f;
	XMMATRIX rotation = XMMatrixRotationZ(convertToRad);
	rotation *= XMMatrixTranslation(mpInstance->World._41, mpInstance->World._42, mpInstance->World._43);
	XMStoreFloat4x4(&mpInstance->World, rotation);
}

void GameObject::SetRotation(const float x, const float y, const float z)
{
	XMMATRIX rotation;

	//X
	float convertToRad = x * 3.14159f / 180.0f;
	rotation = XMMatrixRotationX(convertToRad);

	//Y
	convertToRad = y * 3.14159f / 180.0f;
	rotation *= XMMatrixRotationY(convertToRad);

	//Z
	convertToRad = z * 3.14159f / 180.0f;
	rotation *= XMMatrixRotationZ(convertToRad);

	rotation *= XMMatrixTranslation(mpInstance->World._41, mpInstance->World._42, mpInstance->World._43);
	XMStoreFloat4x4(&mpInstance->World, rotation);
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
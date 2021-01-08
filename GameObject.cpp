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


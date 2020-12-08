#pragma once
#include <DirectXMath.h>

DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b) 
{
	return 	{ a.x * b.x,		a.y * b.y,		a.z * b.z };
}

DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
	return 	{	a.x + b.x,		a.y + b.y,		a.z + b.z	};
}
DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
	return 	{	a.x - b.x,		a.y - b.y,		a.z - b.z	};
}

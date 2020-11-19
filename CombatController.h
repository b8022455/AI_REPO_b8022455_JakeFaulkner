#pragma once

#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "RenderItem.h"

using namespace DirectX;

//class PlayerWeapon
//{
//
//public:
//	PlayerWeapon()	{};	//Contructor
//
//	void CreateWeaponMaterial();
//	void BuildWeaponGeometry(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList);
//	void RenderWeapon(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);
//
//private:
//	int weaponDamage = 25;	//Could replace, depends on later decisions
//	std::unique_ptr<Material> weaponMat;
//	std::unique_ptr<MeshGeometry> weaponGeo;
//};
//
//class CombatController
//{
//public:
//	CombatController() {};		//Constructor
//	void Init();
//	void Update();
//	void Render();
//
//private:
//	PlayerWeapon w;
//};

class CombatController
{
public:
	const float PI = 3.14159265359f;
	float swordRotation = 0.0f * PI / 180.0f;


private:


};
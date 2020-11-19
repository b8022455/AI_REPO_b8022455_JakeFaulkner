#include "CombatController.h"

//void CombatController::Init()
//{
//	//Initialise all the variables that don't involve the creation of the texture, render or material
//
//	
//}
//
//void CombatController::Update()
//{
//
//}
//
//void CombatController::Render()
//{
//
//}
//
//
//void PlayerWeapon::CreateWeaponMaterial()
//{
//}
//
//void PlayerWeapon::BuildWeaponGeometry(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList)
//{
//
//}
//
//void PlayerWeapon::RenderWeapon(std::vector<std::unique_ptr<RenderItem>> &mAllRitems)
//{
//	auto weaponRitem = std::make_unique<RenderItem>();
//	weaponRitem->World = MathHelper::Identity4x4();
//	weaponRitem->TexTransform = MathHelper::Identity4x4();
//	//weaponRitem->ObjCBIndex = ?;
//	weaponRitem->Mat = weaponMat.get();
//	weaponRitem->Geo = weaponGeo.get();
//	weaponRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	weaponRitem->IndexCount = weaponRitem->Geo->DrawArgs["box"].IndexCount;		//Is Box found?
//	weaponRitem->StartIndexLocation = weaponRitem->Geo->DrawArgs["box"].StartIndexLocation;	//Again, see above
//	weaponRitem->BaseVertexLocation = weaponRitem->Geo->DrawArgs["box"].BaseVertexLocation;
//	weaponRitem->Bounds = weaponRitem->Geo->DrawArgs["box"].Bounds;
//
//	mAllRitems.push_back(std::move(weaponRitem));
//}
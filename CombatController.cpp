#include "CombatController.h"

void CombatController::Initialize()
{
	mPlayerWeapon.Initialize();
}

void CombatController::PlayerAttack(std::vector<std::unique_ptr<RenderItem>> &mAllRitems)
{
	mPlayerWeapon.Attack(mAllRitems);
}


void PlayerWeapon::Initialize()
{
	times.timeAtNow = time(0);
	times.nextAtkTime = 0;
	times.err = localtime_s(&times.currentTime, &times.timeAtNow);		//Gets current time from system and stores in currentTime struct

	weaponRotation = 0.0f;
	attacking = false;

}

void PlayerWeapon::Attack(std::vector<std::unique_ptr<RenderItem>> &mAllRitems)
{
	if (times.currentTime.tm_sec > times.nextAtkTime)
	{
		PositionWeapon(mAllRitems);			
	}

	if (attacking)
		SwingWeapon(mAllRitems);

	UpdateTimer();

}

void PlayerWeapon::PositionWeapon(std::vector<std::unique_ptr<RenderItem>> &mAllRitems)
{
	times.nextAtkTime = times.currentTime.tm_sec + times.AttackDelay;	//Resets timer on attack delay (Currentime + 1 sec)
	attacking = true;
	UpdateWeaponMatrix();

	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, weaponPositionMatrix);
	boxRitem->ObjCBIndex = 0;
	boxRitem->Mat = mAllRitems.at(0).get()->Mat;
	boxRitem->Geo = mAllRitems.at(0).get()->Geo;
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	///Stuff thats required to show in instancing projects
	boxRitem->InstanceCount = 0;
	boxRitem->Instances.resize(125);

	mAllRitems.push_back(std::move(boxRitem));
}

void PlayerWeapon::SwingWeapon(std::vector<std::unique_ptr<RenderItem>> &mAllRitems)
{
	if (weaponRotation > PI / 2)
		ResetWeaponPosition(mAllRitems);
	else
	{
		weaponRotation += 0.0698132f;		///Could replace into its own variable
		UpdateWeaponMatrix();		//Updates to show the new rotation

		auto boxRitem = std::make_unique<RenderItem>();			//Updates the position of the object to show the rotation taking place
		XMStoreFloat4x4(&boxRitem->World, weaponPositionMatrix);
		boxRitem->ObjCBIndex = 0;
		boxRitem->Mat = mAllRitems.at(0).get()->Mat;
		boxRitem->Geo = mAllRitems.at(0).get()->Geo;
		boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
		boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
		boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
		///Stuff thats required to show in instancing projects
		boxRitem->InstanceCount = 0;
		boxRitem->Instances.resize(125);

		mAllRitems.push_back(std::move(boxRitem));
	}
}

void PlayerWeapon::ResetWeaponPosition(std::vector<std::unique_ptr<RenderItem>> &mAllRitems)
{
	weaponRotation = 0.0f;
	attacking = false;

	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixTranslation(0.0f, -10.0f, 0.0f));			//Repositions the weapon away from the player as its not in use for now
	boxRitem->ObjCBIndex = 0;
	boxRitem->Mat = mAllRitems.at(0).get()->Mat;
	boxRitem->Geo = mAllRitems.at(0).get()->Geo;
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	///Stuff thats required to show in instancing projects
	boxRitem->InstanceCount = 0;
	boxRitem->Instances.resize(125);

	mAllRitems.push_back(std::move(boxRitem));
}

void PlayerWeapon::UpdateTimer()
{
	times.timeAtNow = time(0);
	times.err = localtime_s(&times.currentTime, &times.timeAtNow);
	if (times.currentTime.tm_sec == 0)	times.nextAtkTime = 0;		//Prevents it from doing multiple attacks per frame once hit 60 secs
}

void PlayerWeapon::UpdateWeaponMatrix()
{
	weaponPositionMatrix = XMMatrixTranslation(0.0f, 0.0f, 5.0f);
	weaponPositionMatrix *= XMMatrixRotationY(weaponRotation);
	weaponPositionMatrix = XMMatrixTranslation(5.0f, 2.0f, 0.0f);
}


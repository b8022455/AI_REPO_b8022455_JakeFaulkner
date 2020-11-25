#include "CombatController.h"

void CombatController::Initialize()
{
	mPlayerWeapon.Initialize();
}

void CombatController::Update(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	isAttacking = CheckIfAttackIsFinished();		//Stops the attack

	if (isAttacking)
		mPlayerWeapon.SwingWeapon(mAllRitems);

	mPlayerWeapon.UpdateTimer();					//Keeps timer updated regardless of key input
}

void CombatController::PlayerAttack(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	mPlayerWeapon.Attack(mAllRitems);
}

bool CombatController::CheckIfAttackIsFinished()
{
	return mPlayerWeapon.GetAttackStatus();
}

void CombatController::DamageEnemy()
{
	//Take health from enemy
}

void CombatController::DamagePlayer()
{
	//Take health from player
}


void PlayerWeapon::Initialize()
{
	times.timeAtNow = time(0);
	times.nextAtkTime = 0;
	times.err = localtime_s(&times.currentTime, &times.timeAtNow);		//Gets current time from system and stores in currentTime struct

	weaponRotation = 0.0f;
	attacking = false;

}

void PlayerWeapon::Attack(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	if (times.currentTime.tm_sec > times.nextAtkTime)
	{
		PositionWeapon(mAllRitems);

		SwingWeapon(mAllRitems);
	}

}

void PlayerWeapon::PositionWeapon(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	times.nextAtkTime = times.currentTime.tm_sec + times.AttackDelay;	//Resets timer on attack delay (Currentime + 1 sec)
	attacking = true;
	weaponRotation = 0.0f;
	UpdateWeaponMatrix();

	//Positions the first instance (Only instance) of the 'weapon' to the correct position
	mAllRitems["Weapon"]->Instances.at(0).MaterialIndex = 1;
	mAllRitems["Weapon"]->Instances.at(0).World = weaponPositionMatrix2;	///In future, change position to be where the player is	
}

void PlayerWeapon::SwingWeapon(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	if (weaponRotation > PI / 2)
		ResetWeaponPosition(mAllRitems);
	else
	{
		weaponRotation += 0.002f;		///Could replace into its own variable
		UpdateWeaponMatrix();		//Updates to show the new rotation

		mAllRitems["Weapon"]->Instances.at(0).World = weaponPositionMatrix2;
	}
}

void PlayerWeapon::ResetWeaponPosition(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	weaponRotation = 0.0f;
	attacking = false;

	UpdateWeaponMatrix();

	mAllRitems["Weapon"]->Instances.at(0).World = weaponPositionMatrix2;
}

void PlayerWeapon::UpdateTimer()
{
	times.timeAtNow = time(0);
	times.err = localtime_s(&times.currentTime, &times.timeAtNow);
	if (times.currentTime.tm_sec == 0)	times.nextAtkTime = 0;		//Prevents it from doing multiple attacks per frame once hit 60 secs
}

void PlayerWeapon::UpdateWeaponMatrix()
{
	///Original matrix used for positioning, need to find way to do rotation around a point using 1 matrix
	weaponPositionMatrix = XMMatrixTranslation(5.0f, 0.0f, 0.0f);		///Could replace with a weapons position variable + 5.0 to get local positions
	weaponPositionMatrix *= XMMatrixRotationY(weaponRotation);
	weaponPositionMatrix *= XMMatrixTranslation(0.0f, 10.0f, 0.0f);

	XMStoreFloat4x4(&weaponPositionMatrix2, weaponPositionMatrix);		//Stores above calculated matrix into the matrix which is assigned to the Objs World position
}

bool PlayerWeapon::GetAttackStatus()
{
	return attacking;
}
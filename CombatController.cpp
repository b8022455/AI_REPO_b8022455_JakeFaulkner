#include "CombatController.h"

void CombatController::Initialize()
{
	mPlayerWeapon.Initialize();
	collisionPoint = mPlayerWeapon.collisionPos;
}

void CombatController::Update(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	isAttacking = CheckIfAttackIsFinished();		//Stops the attack
	collisionPoint = mPlayerWeapon.collisionPos;	//Updates to check positions of objects in case of collision

	if (isAttacking)
		mPlayerWeapon.SwingWeapon(mAllRitems);
	//else												//Code for weapon follwing player at all times, move updateWeaponMatrix to public in order to work
	//{
	//	mPlayerWeapon.UpdateWeaponMatrix(mAllRitems);
	//}

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

bool CombatController::CheckCollision(float ObjX, float ObjY, float ObjZ)
{
	//Gets the distance between both objects coordinates
	float xDistance = ObjX - collisionPoint._41;
	float yDistance = ObjY - collisionPoint._42;
	float zDistance = ObjZ - collisionPoint._43;

	if (xDistance > -1.0f && xDistance < 1.0f)							//If distance between X coordinate is within boundaries (-1.0 < X < 1.0)
		if (yDistance > -1.5f && yDistance < 1.5f)						//If distance between Y coordinate is within boundaries (-1.5 < Y < 1.5)
			if (zDistance > -1.0f && zDistance < 1.0f)					//If distance between Z coordinate is within boundaries (-1.0f < Z < 1.0f)
				return true;				//There is a collision between the 2 objects

	return false;				//If the distance between the objects is not within the boundaries, there is no collision
}

bool CombatController::CheckCollision(XMFLOAT3 Obj1, XMFLOAT3 Obj2)
{
	float xDistance = Obj1.x - Obj2.x;
	float yDistance = Obj1.y - Obj2.y;
	float zDistance = Obj1.z - Obj2.z;

	if (xDistance > -1.0f && xDistance < 1.0f)							//If distance between X coordinate is within boundaries (-1.0 < X < 1.0)
		if (yDistance > -1.5f && yDistance < 1.5f)						//If distance between Y coordinate is within boundaries (-1.5 < Y < 1.5)
			if (zDistance > -1.0f && zDistance < 1.0f)					//If distance between Z coordinate is within boundaries (-1.0f < Z < 1.0f)
				return true;				//There is a collision between the 2 objects

	return false;				//If the distance between the objects is not within the boundaries, there is no collision
}

bool CombatController::CheckCollision(XMFLOAT3 Obj1, XMFLOAT3 Obj2, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
{
	float xDistance = Obj1.x - Obj2.x;
	float yDistance = Obj1.y - Obj2.y;
	float zDistance = Obj1.z - Obj2.z;

	if (xDistance > xMin && xDistance < xMax)							//If distance between X coordinate is within custom boundaries
		if (yDistance > yMin && yDistance < yMax)						//If distance between Y coordinate is within custom boundaries
			if (zDistance > zMin && zDistance < zMax)					//If distance between Z coordinate is within custom boundaries
				return true;				//There is a collision between the 2 objects

	return false;				//If the distance between the objects is not within the boundaries, there is no collision
}


void PlayerWeapon::Initialize()
{
	times.timeAtNow = time(0);
	times.nextAtkTime = 0;
	times.err = localtime_s(&times.currentTime, &times.timeAtNow);		//Gets current time from system and stores in currentTime struct

	weaponRotation = -1.39626f;
	attacking = false;

	//Collision point stuff
	collisionPos = weaponPositionMatrix2;
	collisionPos._41 += 0.1f;

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
	weaponRotation = -1.39626f;		//Sets rotation to default -80 degree angle
	UpdateWeaponMatrix(mAllRitems);

	//Positions the first instance (Only instance) of the 'weapon' to the correct position
	mAllRitems["Weapon"]->Instances.at(0).MaterialIndex = 4;
	mAllRitems["Weapon"]->Instances.at(0).World = weaponPositionMatrix2;
}

void PlayerWeapon::SwingWeapon(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	if (weaponRotation > 1.39626f)		//80 degrees, not final until final sword/player models are in
		ResetWeaponPosition(mAllRitems);
	else
	{
		weaponRotation += 0.03f;		///Could replace into its own variable
		UpdateWeaponMatrix(mAllRitems);		//Updates to show the new rotation

		mAllRitems["Weapon"]->Instances.at(0).World = weaponPositionMatrix2;
	}

	//Sets collision point to the edge of the sword
	collisionPos = weaponPositionMatrix2;
	collisionPos._41 += 0.1f;
}

void PlayerWeapon::ResetWeaponPosition(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	weaponRotation = -1.39626f;			//Resets rotation to -80 degrees
	attacking = false;

	weaponPositionMatrix = XMMatrixTranslation(0.0f, -5.0f, 2.0f);		//Resets the sword back underneath the map until attack is used again
	XMStoreFloat4x4(&weaponPositionMatrix2, weaponPositionMatrix);

	mAllRitems["Weapon"]->Instances.at(0).World = weaponPositionMatrix2;
}

void PlayerWeapon::UpdateTimer()
{
	times.timeAtNow = time(0);
	times.err = localtime_s(&times.currentTime, &times.timeAtNow);
	if (times.currentTime.tm_sec == 0)	times.nextAtkTime = 0;		//Prevents it from doing multiple attacks per frame once hit 60 secs
}

void PlayerWeapon::UpdateWeaponMatrix(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems)
{
	///Find way to do rotation around a point using 1 matrix
	weaponPositionMatrix = XMMatrixTranslation(1.4f, 0.0f, 0.0f);
	weaponPositionMatrix *= XMMatrixRotationY(weaponRotation);
	//Positions the weapon at the players position + offset
	weaponPositionMatrix *= XMMatrixTranslation(mAllRitems["Player"]->Instances.at(0).World._41 + 0.2f, 
		mAllRitems["Player"]->Instances.at(0).World._42 + 1.0f, mAllRitems["Player"]->Instances.at(0).World._43);

	XMStoreFloat4x4(&weaponPositionMatrix2, weaponPositionMatrix);		//Stores above calculated matrix into the matrix which is assigned to the Objs World position
}

bool PlayerWeapon::GetAttackStatus()
{
	return attacking;
}
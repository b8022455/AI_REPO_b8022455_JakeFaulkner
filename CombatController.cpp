#include "CombatController.h"

void CombatController::Initialize(Player* player, PlayerWeapon* playerWeapon, std::vector<Enemy>* enemies)
{
	assert(player && enemies && playerWeapon);

	mpPlayer = player;
	mpEnemies = enemies;
	mpPlayerWeapon = playerWeapon;

	//Sets up the collision point for the weapon
	collisionPoint = mpPlayerWeapon->mpInstance->World;
	collisionPoint._41 += 0.1f;
}

void CombatController::Update()
{
	mpPlayerWeapon->playerPos = mpPlayer->mpInstance->World;
	isAttacking = CheckIfAttackIsFinished();		//Stops the attack

	//Updates the collision point when the weapon is rotating
	collisionPoint = mpPlayerWeapon->mpInstance->World;
	collisionPoint._41 += 0.1f;

	if (isAttacking)
		mpPlayerWeapon->SwingWeapon();
	else
	{
		mpPlayerWeapon->facingLeft = mpPlayer->facingLeft;
		mpPlayerWeapon->facingRight = mpPlayer->facingRight;
	}

	mpPlayerWeapon->UpdateTimer();					//Keeps timer updated regardless of key input
}

void CombatController::PlayerAttack()
{
	mpPlayerWeapon->Attack();
}

bool CombatController::CheckIfAttackIsFinished()
{
	return mpPlayerWeapon->GetAttackStatus();
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

void PlayerWeapon::Initialize(const std::string& renderItemName)
{
	// Setup a render item
	AddRenderItemInstance("Weapon");

	times.timeAtNow = time(0);
	times.nextAtkTime = 0;
	times.storeLocaltime = localtime_s(&times.currentTime, &times.timeAtNow);		//Gets current time from system and stores in currentTime struct

	weaponRotation = weaponStartingRotation;		//Sets to -80 degrees
	attacking = false;
}

void PlayerWeapon::Attack()
{
	if (times.currentTime.tm_sec > times.nextAtkTime)
	{
		PositionWeapon();
		SwingWeapon();
	}
}

void PlayerWeapon::PositionWeapon()
{
	times.nextAtkTime = times.currentTime.tm_sec + times.AttackDelay;	//Resets timer on attack delay (Currentime + 1 sec)
	attacking = true;
	UpdateWeaponMatrix();

	mpInstance->MaterialIndex = 4;
}

void PlayerWeapon::SwingWeapon()
{
	if (weaponRotation > weaponEndRotation)		//80 degrees, not final until final sword/player models are in
		ResetWeaponPosition();
	else
	{
		weaponRotation += weaponIncrementRotationAmount;
		UpdateWeaponMatrix();		//Updates to show the new rotation
	}
}

void PlayerWeapon::ResetWeaponPosition()
{
	weaponRotation = weaponStartingRotation;			//Resets rotation to -80 degrees
	attacking = false;

	weaponPositionMatrix = XMMatrixTranslation(0.0f, -5.0f, 2.0f);		//Resets the sword back underneath the map until attack is used again
	XMStoreFloat4x4(&mpInstance->World, weaponPositionMatrix);
}

void PlayerWeapon::UpdateTimer()
{
	times.timeAtNow = time(0);
	times.storeLocaltime = localtime_s(&times.currentTime, &times.timeAtNow);
	if (times.currentTime.tm_sec == 0)	times.nextAtkTime = 0;		//Prevents it from doing multiple attacks per frame once hit 60 secs
}

void PlayerWeapon::UpdateWeaponMatrix()
{
	float playerPosOffsetX = 0.0f;

	if (facingLeft)
	{
		playerPosOffsetX = -0.2f;
		weaponPositionMatrix = XMMatrixTranslation(-0.5f, 0.0f, 0.0f);
	}
	else if (facingRight)
	{
		playerPosOffsetX = 0.2f;
		weaponPositionMatrix = XMMatrixTranslation(1.4f, 0.0f, 0.0f);
	}

	///Find way to do rotation around a point using 1 matrix
	weaponPositionMatrix *= XMMatrixRotationY(weaponRotation);
	//Positions the weapon at the players position + offset
	weaponPositionMatrix *= XMMatrixTranslation(
		playerPos._41 + playerPosOffsetX,
		playerPos._42 + 1.0f,
		playerPos._43
	);

	XMStoreFloat4x4(&mpInstance->World, weaponPositionMatrix);		//Stores above calculated matrix into the world matrix for the obj
}

bool PlayerWeapon::GetAttackStatus()
{
	return attacking;
}
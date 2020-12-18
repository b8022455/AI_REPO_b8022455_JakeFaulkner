#include "CombatController.h"

void CombatController::Initialize(Player* player, PlayerWeapon* playerWeapon, std::vector<Enemy>* enemies)
{
	assert(player && enemies && playerWeapon);

	mpPlayer = player;
	mpEnemies = enemies;
	mpPlayerWeapon = playerWeapon;
	mpPlayerWeapon->mpInstance->MaterialIndex = 4;

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
		mpPlayerWeapon->playerDirection = mpPlayer->playerDir;		//gets direction the player is facing, doesn't detect when attacking to prevent sword switching positions

		std::for_each(mpEnemies->begin(), mpEnemies->end(), [&](Enemy& e)
		{
			e.playerDirection = mpPlayer->playerDir;				//Gets direction player is facing into the enemy class to correctly blowback the enemy from the player
		});

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

	if (xDistance > -1.5f && xDistance < 1.5f)							//If distance between X coordinate is within boundaries (-1.5 < X < 1.0)
		if (yDistance > -1.5f && yDistance < 1.5f)						//If distance between Y coordinate is within boundaries (-1.5 < Y < 1.5)
			if (zDistance > -1.5f && zDistance < 1.5f)					//If distance between Z coordinate is within boundaries (-1.0f < Z < 2.0f)
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

void CombatController::EquipWeapon(std::string weaponName)
{
	equippedWeapon = weaponName;		//Sets the equipped weapon
	mpPlayer->attack = mpPlayer->attack + mpPlayerWeapon->GetWeaponStats(weaponName);			//Updates player attack value to base attack + equipped weapons attack amount
}

std::string CombatController::GetCurrentWeapon()
{
	return equippedWeapon;
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

	if (playerDirection > 1)		//If the player direction is Up (2 in the enum) or Down (3 in the enum)
	{
		weaponStartingRotation = 1.5708f;		//Sets new starting to rotation to 90 degrees for up/down
		weaponEndRotation = 7.85398f;			//Sets new end rotation to 450 degrees
	}
	else if (playerDirection <= 1)				//If player direction is Left (0 in the enum) or Right (1 in the enum)
	{
		weaponStartingRotation = 0.0f;
		weaponEndRotation = 6.28319f;			//360 degrees
	}

	weaponRotation = weaponStartingRotation;

	UpdateWeaponMatrix();
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
	switch (playerDirection)
	{
	case 0:						//Left
		weaponPositionMatrix = XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
		break;

	case 1:						//Right
		weaponPositionMatrix = XMMatrixRotationZ(3.14159f);
		weaponPositionMatrix *= XMMatrixTranslation(1.0f, 0.0f, 0.0f);
		break;

	case 2:						//Up
		weaponPositionMatrix = XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
		break;

	case 3:						//Down
		weaponPositionMatrix = XMMatrixRotationZ(3.14159f);
		weaponPositionMatrix *= XMMatrixTranslation(1.0f, 0.0f, 0.0f);
		break;

	default:
		assert(playerDirection);
		break;
	}

	///Find way to do rotation around a point using 1 matrix
	weaponPositionMatrix *= XMMatrixRotationY(weaponRotation);
	//Positions the weapon at the players position + offset
	weaponPositionMatrix *= XMMatrixTranslation(
		playerPos._41,
		playerPos._42 + 1.0f,
		playerPos._43
	);

	XMStoreFloat4x4(&mpInstance->World, weaponPositionMatrix);		//Stores above calculated matrix into the world matrix for the obj
}

bool PlayerWeapon::GetAttackStatus()
{
	return attacking;
}

int PlayerWeapon::GetWeaponStats(std::string equippedWeapon)
{
	int attack = 0;
	if (equippedWeapon == "Stick")
	{
		attack = 10;
		mpInstance->MaterialIndex = 4;		//Visual show of weapons being changed when equipped
		///Find way to change model to something else
	}

	if (equippedWeapon == "Leadpipe")
	{
		attack = 20;
		mpInstance->MaterialIndex = 1;
		//etc.
	}

	if (equippedWeapon == "Nail Bat")
	{
		attack = 40;
	}

	if (equippedWeapon == "Plastic Spork")
	{
		attack = 5;
	}

	return attack;
}
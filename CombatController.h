#pragma once

#include <ctime>
#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "AttackTimeDelay.h"

using namespace DirectX;

class PlayerWeapon : public GameObject
{
public:
	PlayerWeapon() {};	//Default Constructor

	void Initialize(const std::string& renderItemName) override;	//Sets up the TimeDelay struct values
	void Attack();
	void Reset();
	void SwingWeapon();				//Swings the weapon based on the rotation var
	void ResetWeaponPosition();
	bool GetAttackStatus();			//Lets the Combat Controller class know when the attack has ended
	void SetDirection(int dir);
	void UpdateTime();

	int GetWeaponStats(std::string equippedWeap);				//Switches attack amount, model etc. based on what the name of the weapon equipped is

private:
	void PositionWeapon();
	void UpdateWeaponMatrix();

	float weaponStartingRotation = 0.0f;		//0 degrees starting point for rotation
	float weaponRotation = 0.0f;
	float weaponEndRotation = 360.f;			//80 degress ending point for rotation
	const float weaponIncrementRotationAmount = 4.f;
	int playerDirection =0;			//Gets enum value of which way player is facing, could remove and define weaponMatrix var in controller

	AttackTimeDelay times;
};

class CombatController
{
public:
	CombatController() {};	//Default Constructor
	//todo pass in player and enemy collection pointer
	void Initialize(Player* player, PlayerWeapon* playerWeapon, std::vector<Enemy>* enemies);
	void Update();
	void Reset();
	void PlayerAttack();	//Connects to PlayerWeapon::Attack() function
	bool CheckIfAttackIsFinished();					//Checks with PlayerWeapon Class to see if its possible to attack again

	std::string GetCurrentWeapon();					//Returns name of current weapon to check if selected weapon is already equipped
	void EquipWeapon(std::string weaponName);		//Passes into SwitchWeapon in playerWeapon, used to get new attack value for player (base + weapon)

	XMFLOAT4X4 collisionPoint;
private:
	PlayerWeapon* mpPlayerWeapon;
	Player* mpPlayer = nullptr;
	std::vector<Enemy>* mpEnemies = nullptr; //Could be more generic with GameObject. Change collection type to state
	const float PI = 3.14159265359f;
	bool isAttacking = false;
	std::string equippedWeapon = "Stick";		//Default weapon equipped
};
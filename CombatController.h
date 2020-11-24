#pragma once

#include <ctime>

#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "RenderItem.h"

using namespace DirectX;

class PlayerWeapon
{
public:
	PlayerWeapon() {};	//Default Constructor

	void Initialize();		//Sets up the TimeDelay struct values
	void Attack(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);
	void SwingWeapon(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);		//Swings the weapon based on the rotation var
	bool GetAttackStatus();		//Lets the Combat Controller class know when the attack has ended
	void UpdateTimer();		//Keeps track of the time delay for attacking

private:
	void PositionWeapon(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);	///Positions weapon at the position of the player, For now just positions at random place in scene, Fix once player model is in!!!
	void ResetWeaponPosition(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);		///Positions weapon out of sight when done swinging, Find better way to do this!!!
	void UpdateWeaponMatrix();

	int damage;		///Not sure if needed in future
	XMMATRIX weaponPositionMatrix;		//Could change from XMMATRIX into different type?
	XMFLOAT4X4 weaponPositionMatrix2;		//Matrix being used for positioning, ignore above line
	float weaponRotation;
	const float PI = 3.14159265359f;			///Could position in CombatController class if being used for more things
	bool attacking;

	struct TimeDelay		//All variables used in the timing stuff, kept in struct to keep them close together
	{
		time_t timeAtNow;
		struct tm currentTime;
		int nextAtkTime;
		const int AttackDelay = 1;	//1 Second
		errno_t err;			///Need a better name!!!
	};

	TimeDelay times;

	///Future variables?
	//Position of weapon in world to check collision with enemies
};

class CombatController
{
public:
	CombatController() {};	//Default Constructor
	void Initialize();		///Will be more useful in the future
	void Update(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);
	void PlayerAttack(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);	//Connects to PlayerWeapon::Attack() function
	bool CheckIfAttackIsFinished();					//Checks with PlayerWeapon Class to see if its possible to attack again

private:
	void DamageEnemy();		///For future use?
	void DamagePlayer();

	PlayerWeapon mPlayerWeapon;
	bool isAttacking = false;
};
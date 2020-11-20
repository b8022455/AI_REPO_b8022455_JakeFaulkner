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

private:
	void PositionWeapon(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);	///Positions weapon at the position of the player, For now just positions at random place in scene, Fix once player model is in!!!
	void SwingWeapon(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);		//Swings the weapon based on the rotation var
	void ResetWeaponPosition(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);		///Positions weapon out of sight when done swinging, Find better way to do this!!!
	void UpdateTimer();		//Keeps track of the time delay for attacking
	void UpdateWeaponMatrix();

	int damage;		///Not sure if needed in future
	XMMATRIX weaponPositionMatrix;		//Could change from XMMATRIX into different type?
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
};

class CombatController
{
public:
	CombatController() {};	//Default Constructor
	void Initialize();		///Will be more useful in the future
	void PlayerAttack(std::vector<std::unique_ptr<RenderItem>> &mAllRitems);	//Connects to PlayerWeapon::Attack() function

private:
	PlayerWeapon mPlayerWeapon;
};
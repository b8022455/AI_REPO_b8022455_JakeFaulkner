#pragma once

#include <ctime>
#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include <vector>
#include "Player.h"
#include "Enemy.h"

using namespace DirectX;

class PlayerWeapon : public GameObject
{
public:
	PlayerWeapon() {};	//Default Constructor

	void Initialize(const std::string& renderItemName) override;	//Sets up the TimeDelay struct values
	void Attack();
	void SwingWeapon();				//Swings the weapon based on the rotation var
	bool GetAttackStatus();			//Lets the Combat Controller class know when the attack has ended
	void UpdateTimer();				//Keeps track of the time delay for attacking

	XMFLOAT4X4 collisionPos;		//Passed to Combat Controller class for ease of access for now
private:
	void PositionWeapon		();	///Positions weapon at the position of the player, For now just positions at random place in scene, Fix once player model is in!!!
	void ResetWeaponPosition();		///Positions weapon out of sight when done swinging, Find better way to do this!!!
	void UpdateWeaponMatrix	();

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
	//todo pass in player and enemy collection pointer
	void Initialize(Player* player, PlayerWeapon* playerWeapon , std::vector<Enemy>* enemies);		///Will be more useful in the future
	void Update();
	void PlayerAttack();	//Connects to PlayerWeapon::Attack() function
	bool CheckIfAttackIsFinished();					//Checks with PlayerWeapon Class to see if its possible to attack again
	bool CheckCollision(float ObjX, float ObjY, float ObjZ);		//Used specifically for weapon and enemy collision
	bool CheckCollision(XMFLOAT3 Object1, XMFLOAT3 Object2);								//Can be generically used for any type of collision involving 2 objs, need to move it somewhere outside of class
	bool CheckCollision(XMFLOAT3 Object1, XMFLOAT3 Object2, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);		//Generic for 2 objects collision but includes changeable coordinate boundaries, again move it out of class

	XMFLOAT4X4 collisionPoint;		//Used in the check collision functions, !!! in future change this to only have this variable in playerWeapon
private:
	void DamageEnemy();		///For future use?
	void DamagePlayer();

	PlayerWeapon* mpPlayerWeapon;
	Player* mpPlayer;
	std::vector<Enemy>* mpEnemies; //Could be more generic with GameObject. Change collection type to state
	bool isAttacking = false;
};
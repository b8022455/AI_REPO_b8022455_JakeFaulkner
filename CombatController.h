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
	void SwingWeapon();				//Swings the weapon based on the rotation var
	bool GetAttackStatus();			//Lets the Combat Controller class know when the attack has ended
	void UpdateTimer();				//Keeps track of the time delay for attacking

	int GetWeaponStats(std::string equippedWeap);				//Switches attack amount, model etc. based on what the name of the weapon equipped is

	//XMFLOAT4X4 collisionPos;		//Passed to Combat Controller class for ease of access for now
	XMFLOAT4X4 playerPos;

	int playerDirection;			//Gets enum value of which way player is facing

private:
	void PositionWeapon();
	void ResetWeaponPosition();		///Positions weapon out of sight when done swinging, Find better way to do this!!!
	void UpdateWeaponMatrix();

	int damage;		///Not sure if needed in future
	XMMATRIX weaponPositionMatrix;
	float weaponStartingRotation = 0.0f;		//0 degrees starting point for rotation
	float weaponRotation;
	float weaponEndRotation = 6.28319f;			//80 degress ending point for rotation
	const float weaponIncrementRotationAmount = 0.05f;

	AttackTimeDelay times;
};

class CombatController
{
public:
	CombatController() {};	//Default Constructor
	//todo pass in player and enemy collection pointer
	void Initialize(Player* player, PlayerWeapon* playerWeapon, std::vector<Enemy>* enemies);		///Will be more useful in the future
	void Update();
	void PlayerAttack();	//Connects to PlayerWeapon::Attack() function
	bool CheckIfAttackIsFinished();					//Checks with PlayerWeapon Class to see if its possible to attack again
	bool CheckCollision(float ObjX, float ObjY, float ObjZ);		//Used specifically for weapon and enemy collision
	bool CheckCollision(XMFLOAT3 Object1, XMFLOAT3 Object2);								//Can be generically used for any type of collision involving 2 objs, need to move it somewhere outside of class
	bool CheckCollision(XMFLOAT3 Object1, XMFLOAT3 Object2, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);		//Generic for 2 objects collision but includes changeable coordinate boundaries, again move it out of class

	std::string GetCurrentWeapon();					//Returns name of current weapon to check if selected weapon is already equipped
	void EquipWeapon(std::string weaponName);		//Passes into SwitchWeapon in playerWeapon, used to get new attack value for player (base + weapon)

	XMFLOAT4X4 collisionPoint;
private:
	void DamageEnemy();		///For future use?
	void DamagePlayer();

	PlayerWeapon* mpPlayerWeapon;
	Player* mpPlayer;
	std::vector<Enemy>* mpEnemies; //Could be more generic with GameObject. Change collection type to state
	const float PI = 3.14159265359f;
	bool isAttacking = false;
	std::string equippedWeapon = "Stick";		//Default weapon equipped
};
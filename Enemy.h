#pragma once
#include "GameObject.h"
#include "RenderItem.h"
#include "Constants.h"
//#include "Item.h"			//Need to decide if it can be put in another file rather than its own
#include <random>
#include "Constants.h"
#include "AttackTimeDelay.h"

class Enemy : public GameObject
{
public:

	Enemy(const std::string& type, int att)			//Gives enemy its type
	{
		mEnemyType = type;
		mpDropItems = &GC::ITEM_LOOKUP_ENEMIES.at(mEnemyType);
		assert(mpDropItems);
		attack = att;
	};

	void InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int materialIndex);	//Sets up the enemy
	void SetPosition(const DirectX::XMFLOAT3& newPosition);										//Sets position to something new locally and globally
	void Enemy::SetRandomPosition();
	void DamageEnemy(int damage);						//When enemy gets hit by weapon
	DirectX::XMFLOAT3 GetPosition();					//Returns local instance of position
	const std::string GetDropItem();
	int GetHealth() { return mHealth; }

	void Update(const GameTimer& gt);
	void StartAttack();
	void UpdateAttack();
	DirectX::XMFLOAT3 GetCollisionPoint();


	int playerDirection;								//Gets enum value of which way player is facing
	DirectX::XMFLOAT4X4 attackCollisionPoint;
	int attack;
	AttackTimeDelay times;
private:
	void UpdatePosition();								//Updates local instance of position each update
	int GetRandomValue(int min, int max);				//Gets random value of spawning enemy position & loot drops

	int mHealth = 1;
	std::string mEnemyType; // for item lookup

	//Order of items is from Most Common -> Rarest
	const InventoryUnordered* mpDropItems;


};
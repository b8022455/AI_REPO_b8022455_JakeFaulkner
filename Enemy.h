#pragma once
#include "GameObject.h"
#include "RenderItem.h"
#include "Constants.h"
#include <random>
#include "Constants.h"
#include "AttackTimeDelay.h"
#include "DeltaTimer.h"
#include "EnemyParticle.h"

class Enemy : public GameObject
{
public:

	Enemy(const std::string& type, int att)			//Gives enemy its type
	{
		mEnemyType = type;
		mpDropItems = &GC::ITEM_LOOKUP_ENEMIES.at(mEnemyType);
		assert(mpDropItems);
		mAttack = att;
	};

	void InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int materialIndex);	//Sets up the enemy
	void SetPosition(const DirectX::XMFLOAT3& newPosition);										//Sets position to something new locally and globally
	void Enemy::SetRandomPosition();
	void SetDirection(int dir);
	void DamageEnemy(int damage);						//When enemy gets hit by weapon
	DirectX::XMFLOAT3 GetPosition();					//Returns local instance of position
	const std::string GetDropItem();
	int GetHealth() { return mHealth; }
	int GetAttack() { return mAttack; }

	string GetType() { return mEnemyType; }

	void Update(const GameTimer& gt);
	void Delete();			//Called when an enemy is defeated, removes the particles
	void UpdateAttack();

	DirectX::XMFLOAT3 BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);		//Gets enemy position once bounceback has taken effect

	std::vector<EnemyParticle> particles;

private:
	int GetRandomValue(int min, int max);				//Gets random value of spawning enemy position & loot drops

	int mHealth = 10; // setting to 10 as player attack is 10 by default
	int mAttack;
	std::string mEnemyType; // for item lookup
	int playerDirection;								//Gets enum value of which way player is facing

	//Order of items is from Most Common -> Rarest
	const InventoryUnordered* mpDropItems;
	AttackTimeDelay times;
};
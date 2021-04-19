#pragma once
#include "GameObject.h"
#include "RenderItem.h"
#include "Constants.h"
#include <random>
#include "Constants.h"
#include "DeltaTimer.h"
#include "EnemyParticle.h"

class Enemy : public GameObject
{
public:

	enum Behaviour{NONE,CHASE};

	Enemy(const std::string& type, int att)			//Gives enemy its type
	{
		mpDropItems = &GC::ITEM_LOOKUP_ENEMIES.at(type);
		assert(mpDropItems);
		mAttack = att;
	};

	Enemy()	{};		//Default Constructor

	void Enemy::SetRandomPosition();
	void SetDirection(DirectX::XMFLOAT3 dir);
	void DamageEnemy(int damage);						//When enemy gets hit by weapon
	const std::string GetDropItem();
	int GetHealth() { return mHealth; }
	int GetAttack() { return mAttack; }
	std::string GetType() { return mEnemyType; }
	bool GetIfCanAttack() { return canAttack; }
	float GetEnemyRange() { return mEnemySightRange; }

	void Update(const GameTimer& gt);
	void Reset();
	void MoveOffScreen();			//Called when an enemy is defeated, removes the particles
	void UpdateAttack(float dt);

	DirectX::XMFLOAT3 BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);		//Gets enemy position once bounceback has taken effect

	std::vector<EnemyParticle> particles;
	void SetVelocity(const DirectX::SimpleMath::Vector3 target, const GameTimer& gt);
	void SetHealth(int health); // ONLY USED WHEN ENEMY IS FIRST INITIALISED 

	Behaviour mBehaviour = Behaviour::NONE;

	//Genetic Algorithm Related Functions
	int GetFitnessValue() { return fitnessValue; }
	void IncrementFitnessValue() { fitnessValue++; }
	void GetInitialGenetics(const std::vector<int> randomGenes);		//Passes in genetic information for initial candidates
	void GetInheritedGenetics(const std::vector<int> inheritedGenes);	//Passes in genetic information for offspring candidates
	std::vector<int> GetChromosomes() { return chromosomes; }

private:
	int GetRandomValue(int min, int max);				//Gets random value of spawning enemy position & loot drops

	int mHealth;
	int mAttack;
	float mAttackDelay;
	float mEnemySightRange;
	std::string mEnemyType;
	DirectX::XMFLOAT3 playerDirection;								//Gets enum value of which way player is facing

	//Order of items is from Most Common -> Rarest
	const InventoryUnordered* mpDropItems;

	DeltaTimer mEnemyAttackTimer;
	bool canAttack = false;
	float mAttackDuration;

	DirectX::SimpleMath::Vector3 mVelocity;
	float mSpeed = 1.0f; // current speed - LEAVE AS IS, CHANGES & ISN'T STATIC
	float mDrag = 1.0f; // speed to add (* deltatime in enemy update)


	//Genetic Information Variables
	std::vector<int> chromosomes;		//Health | AttackDelay | MovementSpeed | Enemy Sight Distance | EnemyType | Attack Duration
	int fitnessValue = 0;				//Increments every time candidate damages the player
};
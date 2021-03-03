#pragma once
#include "GameObject.h"
#include "RenderItem.h"
#include "Constants.h"
#include <random>
#include "Constants.h"
#include "DeltaTimer.h"
#include "EnemyParticle.h"
#include "GeneticInformation.h"

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

	Enemy()		//Constructor to get the initial candidates of the population (Uses random genetics)
	{
		GetRandomGenetics();		//Get random values for genetic information

		SetRandomPosition();
	};

	Enemy(Enemy parent1, Enemy parent2)			//Constructor to inherit genetic information from parents
	{
		//Cannot loop through each genetic information part when its a class structure?

		for (int i = 0; i < chromosomes.size(); i++)
		{
			int rand = GetRandomInt(0, 100);		//Get random probability whether to inherit from 1st parent (45%), 2nd parent (45%) or mutate (10%)
			
			if (rand < 45)		//Inherit from 1st parent
			{
				chromosomes.at(i) = parent1.chromosomes.at(i);
			}

			else if (rand < 90)	//Inherit from 2nd parent
			{
				chromosomes.at(i) = parent2.chromosomes.at(i);
			}

			//else
				//Mutate function
		}

		//Function to assign new genetic information

	};

	//void InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int materialIndex);	//Sets up the enemy
	void Enemy::SetRandomPosition();
	void SetDirection(DirectX::XMFLOAT3 dir);
	void DamageEnemy(int damage);						//When enemy gets hit by weapon
	const std::string GetDropItem();
	int GetHealth() { return mHealth; }
	int GetAttack() { return mAttack; }
	bool GetIfCanAttack() { return canAttack; }
	GeneticInformation GetGenetics() { return enemyGenetics; }

	void Update(const GameTimer& gt);
	void Reset();
	void MoveOffScreen();			//Called when an enemy is defeated, removes the particles
	void UpdateAttack(float dt);

	DirectX::XMFLOAT3 BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);		//Gets enemy position once bounceback has taken effect

	std::vector<EnemyParticle> particles;
	void SetVelocity(const DirectX::SimpleMath::Vector3 target, const GameTimer& gt);
	void SetHealth(int health); // ONLY USED WHEN ENEMY IS FIRST INITIALISED 

	Behaviour mBehaviour = Behaviour::NONE;

	//Genetic Algorithm
	int GetFitnessValue() { return fitnessValue; }
	void IncrementFitnessValue() { fitnessValue++; }
	void GetRandomGenetics();		//Gets random genetic information for initial candidates

	int GetRandomInt(int min, int max)
	{
		std::random_device rd;
		std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(min, max);

		return uni(rng);
	};

private:
	int GetRandomValue(int min, int max);				//Gets random value of spawning enemy position & loot drops

	int mHealth;
	int mAttack;
	std::string mEnemyType;
	DirectX::XMFLOAT3 playerDirection;								//Gets enum value of which way player is facing

	//Order of items is from Most Common -> Rarest
	const InventoryUnordered* mpDropItems;

	DeltaTimer mEnemyAttackTimer;
	bool canAttack = false;
	float mAttackDuration;

	//Genetic Information Variables
	std::vector<int> chromosomes;		//Health | AttackDelay | MovementSpeed | EnemyType

	//Stores the genetic information for each enemy (variables which will be different for each enemy + have potential to mutate)
	GeneticInformation enemyGenetics;
	int fitnessValue = 0;		//Increments when enemy damages the player

	DirectX::SimpleMath::Vector3 mVelocity;
	float mSpeed = 1.0f; // current speed - LEAVE AS IS, CHANGES & ISN'T STATIC
	//float mMaxSpeed = 1.0f; // max speed
	float mDrag = 1.0f; // speed to add (* deltatime in enemy update)

};
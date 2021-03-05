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

	Enemy(Enemy& parent1, Enemy& parent2)			//Constructor to inherit genetic information from parents
	{
		//Include other stuff to set up the enemy like model, random pos etc in here as well

		for (int i = 0; i < parent1.chromosomes.size(); i++)
		{
			int rand = GetRandomInt(0, 100);		//Get random probability whether to inherit from 1st parent (45%), 2nd parent (45%) or mutate (10%)
			
			if (rand < 45)		//Inherit from 1st parent
			{
				chromosomes.push_back(parent1.chromosomes.at(i));
			}

			else if (rand < 90)	//Inherit from 2nd parent
			{
				chromosomes.push_back(parent2.chromosomes.at(i));
			}

			else
			{
				if (i != 3)		//Prevents mutation happening on enemy type, type cannot be mutated
					chromosomes.push_back(MutateGenetics());
				else
					chromosomes.push_back(parent1.chromosomes.at(i));		//Pass on type as it cannot be mutated

			}
		}

		//Set up model and rest of non-genetic information
		if (chromosomes.at(3) == 1)		//Enemy type 1
		{
			Initialize("EnemyGhoul");
			particles.resize(20);
			mEnemyType = GC::ENEMY_TYPE_1;
			mpDropItems = &GC::ITEM_LOOKUP_ENEMIES.at("EnemyType1");
		}
		else
		{
			Initialize(GC::GO_ENEMY);
			mEnemyType = GC::ENEMY_TYPE_2;
			mpDropItems = &GC::ITEM_LOOKUP_ENEMIES.at("EnemyType2");
		}

		mAttack = 1;
		mHealth = chromosomes.at(0);
		mSpeed = static_cast<float>(chromosomes.at(2)) * 0.05f;
		assert(mpDropItems);

		SetRandomPosition();

	};

	//void InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int materialIndex);	//Sets up the enemy
	void Enemy::SetRandomPosition();
	void SetDirection(DirectX::XMFLOAT3 dir);
	void DamageEnemy(int damage);						//When enemy gets hit by weapon
	const std::string GetDropItem();
	int GetHealth() { return mHealth; }
	int GetAttack() { return mAttack; }
	std::string GetType() { return mEnemyType; }
	bool GetIfCanAttack() { return canAttack; }

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
	int MutateGenetics();

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
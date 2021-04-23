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

	enum Behaviour{NONE,CHASE,WAIT,SHUFFLE,ROAM,}; // IMPLEMENT OTHER STATES (COWARDLY IF TIME AVAILABLE)

	Enemy(const std::string& type, int att)			//Gives enemy its type
	{
		mEnemyType = type;
		mpDropItems = &GC::ITEM_LOOKUP_ENEMIES.at(mEnemyType);
		assert(mpDropItems);
		mAttack = att;
	};

	//void InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int materialIndex);	//Sets up the enemy
	void Enemy::SetRandomPosition();
	void SetDirection(DirectX::XMFLOAT3 dir);
	void DamageEnemy(int damage);						//When enemy gets hit by weapon
	const std::string GetDropItem();
	int GetHealth() { return mHealth; }
	int GetAttack() { return mAttack; }
	std::string& GetType() { return mEnemyType; }
	bool GetIfCanAttack() { return canAttack; }

	void Update(const GameTimer& gt);
	void Reset();
	void MoveOffScreen();			//Called when an enemy is defeated, removes the particles
	void UpdateAttack(float dt);

	DirectX::XMFLOAT3 BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);		//Gets enemy position once bounceback has taken effect

	std::vector<EnemyParticle> particles;
	void SetVelocity(const DirectX::SimpleMath::Vector3 target, const GameTimer& gt);
	void SetHealth(int health); // ONLY USED WHEN ENEMY IS FIRST INITIALISED 

	int timeCycle = 0; // timecycle affects aggression by adding 10% to more aggressive state (not CHASE, just ROAM)
	bool player_proximity = false; // whether the player is in range
	int lastchase = 0; // adds up previous chase states, the longer the player is in range the more likely it becomes
	Behaviour previous; // behaviour thats being exited, reduces chance of 2 in a row except chase or chase + midnight?
	Behaviour mBehaviour = Behaviour::WAIT;
	DirectX::XMFLOAT3 dir = { 0.0f, 0.0f , 0.0f }; // used for selecting SHUFFLE & ROAM directions
	float shuffleTimer = 0.0f; // used in shuffle wait loop to time jumps between shuffle & wait
	bool wait = false; // used in shuffle wait loop used to determine whether or not to reset the shuffle timer
	bool newdir = false; // used in shuffle wait loop & roam to choose whether set a new direction
private:
	int GetRandomValue(int min, int max);				//Gets random value of spawning enemy position & loot drops

	float stateTimer = 0.0f; // used to change to state NONE
	int base_aggression = ((rand() % 5) + 1); // set at random 1-5 on initialisation, 5 most aggressive, 1 least agressive
	// maybe have time of day add to base aggression?
	// 0-5 base aggression + time of day for a max of 9??

	// dawn				- wait-shuffle, roam	- chase (proximity needed)
	// afternoon		- wait-shuffle, roam	- chase (proximity needed)
	// dusk/evening		- wait-shuffle, roam	- chase (proximity needed)
	// midnight/pitch	- wait-shuffle, roam, chase

	// base aggression adds 10% to chase prob? & 10% to roam if player not in range
	// player prox adds 50% to chase probablity? or just unlocks the possibility of chase
	// roam should not be possible if the player is within range??
	// chase should only be available if within range, midnight does give total range making it a possible base state
	// time of day increases restlessness, increasing roam & shuffle

	int mHealth;
	int mAttack;
	std::string mEnemyType; // for item lookup
	DirectX::XMFLOAT3 playerDirection;								//Gets enum value of which way player is facing

	//Order of items is from Most Common -> Rarest
	const InventoryUnordered* mpDropItems;

	DeltaTimer mEnemyAttackTimer;
	bool canAttack = false;
	float mAttackDuration;


	DirectX::SimpleMath::Vector3 mVelocity;
	float mSpeed = 1.0f; // current speed, used for basic movement
	//float mMaxSpeed = 1.0f; // max speed
	float mDrag = 1.0f; // speed to add (* deltatime in enemy update)

	float mStumble = 1.0f; // shuffle & roam?? speed - MAY NOT BE NEEDED

};
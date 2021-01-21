#pragma once
#include "GameObject.h"
#include "RenderItem.h"
#include "Constants.h"
#include <random>
#include "Constants.h"
#include "AttackTimeDelay.h"

class EnemyParticle : public GameObject		//Stored in here for now in case it could be used for other things
{
public:
	EnemyParticle()		//Constructor
	{
		this->Initialize("particle");
		this->mpInstance->MaterialIndex = 5;
		this->SetPos(DirectX::XMFLOAT3(0.0f, -100.0f, 0.0f));		//Just a starting position thats not in the way
	};

	void Effect(DirectX::XMFLOAT3 enemyPos, const float angle)
	{
		//Rotates the particles around enemies position
		mpInstance->World._41 = enemyPos.x;
		mpInstance->World._42 = enemyPos.y + 0.8f;
		mpInstance->World._43 = enemyPos.z;

		DirectX::XMMATRIX transformation = DirectX::XMMatrixTranslation(xOffset + particleDropPosition, 0.0f, zOffset);
		transformation *= DirectX::XMMatrixScaling(0.4f, 0.4f, 0.4f);
		RotationAroundPoint(transformation, -angle, DirectX::XMFLOAT3(mpInstance->World._41, mpInstance->World._42 - particleDropPosition, mpInstance->World._43));

		//Moves particle diagonally to the ground
		particleDropPosition += particleDropSpeed;

		if (mpInstance->World._42 < 0)		//If particle reaches the floor, reset its position to do the drop again, prevents having more particles in the vector
		{
			particleDropPosition = 0.1f;
		}
	};

	void RemoveEffect()		//Hides effect if enemy attack is not active
	{
		mpInstance->World._42 = -100.0f;
		particleDropPosition = 0.1f;
	};
	
	float particleDropSpeed = static_cast<float>(rand() % 50) / 1000 + 0.02f;		//Speed of which the particle falls to the floor, is random for each particle
	float particleDropPosition = 0.1f;

	//Position of each particle from the enemy, is random for each particle
	float xOffset = static_cast<float>(rand() % 10) / 10 + 1.0f;
	float zOffset = static_cast<float>(rand() % 10) / 10 - 0.2f;
};


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

	void Update(const GameTimer& gt);
	void Delete();			//Called when an enemy is defeated, removes the particles
	void UpdateAttack();

	DirectX::XMFLOAT3 BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);		//Gets enemy position once bounceback has taken effect

	std::vector<EnemyParticle> particles;

private:
	int GetRandomValue(int min, int max);				//Gets random value of spawning enemy position & loot drops

	int mHealth = 100;
	int mAttack;
	std::string mEnemyType; // for item lookup
	int playerDirection;								//Gets enum value of which way player is facing

	//Order of items is from Most Common -> Rarest
	const InventoryUnordered* mpDropItems;
	AttackTimeDelay times;
};
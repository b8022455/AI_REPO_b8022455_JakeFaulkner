#pragma once
#include "GameObject.h"
#include "RenderItem.h"
#include <random>

class Enemy : public GameObject
{
public:
	Enemy() {};		//Default Constructor

	Enemy(int type)			//Gives enemy its type
	{
		enemyType = type;
	};

	void InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int materialIndex);	//Sets up the enemy
	void SetPosition(const DirectX::XMFLOAT3& newPosition);										//Sets position to something new locally and globally
	void Enemy::SetRandomPosition();
	void DamageEnemy(int damage);		//When enemy gets hit by weapon
	DirectX::XMFLOAT3 GetPosition();														//Returns local instance of position
	std::string GetDropItem();
	int GetHealth() { return health; }

	int playerDirection;			//Gets enum value of which way player is facing
private:
	void UpdatePosition();				//Updates local instance of position each update

	int health = 1;
	int enemyType;
	
	//Order of items is from Most Common -> Rarest
	std::vector<std::pair<std::string, std::vector<std::string>>> lookupTable = 
	{
		{"EnemyType1", {"Common Sword", "Potion", "Uncommon Sword", "Elysian Whip", "Rare Sword"}},
		{"EnemyType2", {"Potion", "Potion", "Uncommon Sword", "Rare Sword", "Magical Seeds"}}
	};

};
#pragma once
#include "GameObject.h"
#include "RenderItem.h"
#include <random>

class Enemy : public GameObject
{
public:
	Enemy() {};		//Default Constructor

	void InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int materialIndex);	//Sets up the enemy
	void SetPosition(const DirectX::XMFLOAT3& newPosition);										//Sets position to something new locally and globally
	void Enemy::SetRandomPosition();
	void DamageEnemy(int damage);		//When enemy gets hit by weapon
	DirectX::XMFLOAT3 GetPosition();														//Returns local instance of position
	int playerDirection;			//Gets enum value of which way player is facing
	int health = 1;
	std::string GetDropItem();
private:
	void UpdatePosition();				//Updates local instance of position each update

	int enemyType = 0;			///Can be changed to string to make it easier to understand, used to get correct drops from table
	
	//Order of items is from Most Common -> Rarest
	std::vector<std::pair<std::string, std::vector<std::string>>> lookupTable = 
	{
		{"EnemyType1", {"Common Sword", "Potion", "Uncommon Sword", "Elysian Whip", "Rare Sword"}},
		{"EnemyType2", {"Item1", "Item2", "Item3", "Item4", "Item5"}}
	};




	//std::pair<std::string, std::vector<std::string>> lookUpTable = 
	//{ 
	//	"EnemyType1", {"Common Sword", "Uncommon Sword", "Rare Sword", "Potion", "Elysian Whip"}
	//};
};
#pragma once
#include "GameObject.h"
#include "RenderItem.h"
#include "Constants.h"
//#include "Item.h"			//Need to decide if it can be put in another file rather than its own
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
	Item GetDropItem();
	int GetHealth() { return health; }

	int playerDirection;			//Gets enum value of which way player is facing
private:
	void UpdatePosition();				//Updates local instance of position each update
	int GetRandomValue(int min, int max);				//Gets random value of spawning enemy position & loot drops

	int health = 1;
	int enemyType;

	//Order of items is from Most Common -> Rarest
	std::vector<std::pair<std::string, std::vector<Item>>> lookupTable =
	{
		{
			"EnemyType1",										//Type of the enemy, not used but makes it easier to understand which enemy drops what
			{
				{"Empty"},										//All Items enemys can drop
				{"Potion", ItemCategory::Healing, 2.5},			//heals like 1/4 of health
				{"Leadpipe", ItemCategory::Weapons, 3.5},		//Damage can be middling
				{"Super Potion", ItemCategory::Healing, 5.0},	//heals like 1/2 of health
				{"Nail Bat", ItemCategory::Weapons, 10.0}		//Damage can be high
			}
		},

		{
			"EnemyType2",
			{
				{"Empty"},
				{"Potion", ItemCategory::Healing, 2.5},
				{"Plastic Spork", ItemCategory::Weapons, 0.5},	//Damage can be v low
				{"Holy Water", ItemCategory::Farming, 12.0},	//Helps plants grow faster
				{"Magical Seeds", ItemCategory::Farming, 20.0}	//Grows some good plants?
			}
		},

		{
			"EnemyType:Boss",
			{
				{"Key To Valhalla", ItemCategory::KeyItems, 0.0},		///Find way to ensure this type drops the item!!!
				{"Key To Valhalla", ItemCategory::KeyItems, 0.0},
				{"Key To Valhalla", ItemCategory::KeyItems, 0.0},
				{"Key To Valhalla", ItemCategory::KeyItems, 0.0},
				{"Key To Valhalla", ItemCategory::KeyItems, 0.0},
			}
		}
	};
};
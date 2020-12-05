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
	DirectX::XMFLOAT3 GetPosition();														//Returns local instance of position
private:
	void UpdatePosition();				//Updates local instance of position each update
	
	int health = 100;
};
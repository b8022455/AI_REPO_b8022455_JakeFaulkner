#pragma once
#include "GameObject.h"
#include "RenderItem.h"

class Enemy : public GameObject
{
public:
	Enemy() {};		//Default Constructor

	void InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int materialIndex, std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems);		//Sets up the enemy
	void SetPosition(DirectX::XMFLOAT3 newPosition, std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems);	//Sets position to something new locally and globally
	DirectX::XMFLOAT3 GetPosition();		//Returns local instance of position
	void AddAnotherEnemy(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems);					//Creates another instance of enemy, can be used for spawning

private:
	void UpdatePosition(std::unordered_map<std::string, std::unique_ptr<RenderItem>> &mAllRitems);				//Updates local instance of position each update
	
	int health = 100;
	DirectX::XMFLOAT3 position;		//Stores position from the renderItem world matrix
};
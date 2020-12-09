#include "Enemy.h"

void Enemy::InitEnemyPosition(int instance, DirectX::XMFLOAT3 position, int matIndex)
{
	mpInstance->MaterialIndex = matIndex;
	mpInstance->World._41 = position.x;
	mpInstance->World._42 = position.y;
	mpInstance->World._43 = position.z;
}

void Enemy::SetPosition(const DirectX::XMFLOAT3& newPosition)
{
	//Updates position on the object
	mpInstance->World._41 = newPosition.x;
	mpInstance->World._42 = newPosition.y;
	mpInstance->World._43 = newPosition.z;
}

void Enemy::SetRandomPosition()
{
	int min = -15;
	int max = 15;
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased

	float random_integer = (float)uni(rng);
	float random_integer2 = (float)uni(rng);

	mpInstance->World._41 = random_integer;
	mpInstance->World._42 = 0;
	mpInstance->World._43 = random_integer2;
}

DirectX::XMFLOAT3 Enemy::GetPosition()
{
	return {
		mpInstance->World._41,
		mpInstance->World._42,
		mpInstance->World._43
	};
}

void Enemy::DamageEnemy(int dmg)
{
	health -= dmg;
	mpInstance->MaterialIndex = 5;		//Visual check, remove eventually

	float x = 0.0f;
	float z = 0.0f;

	//Blows back enemy based on what position the enemy was hit from
	switch (playerDirection)
	{
	case 0:										//Left
		x = -5.0f;
		break;

	case 1:										//Right
		x = 5.0f;
		break;

	case 2:										//Up
		z = 5.0f;
		break;

	case 3:										//Down
		z = -5.0f;
		break;
	}

	mpInstance->World._41 += x;
	mpInstance->World._43 += z;
}

std::string Enemy::GetDropItem()
{
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, 100); // guaranteed unbiased

	int drop = uni(rng);		//Generates number between 1 and 100

	std::string droppedItem;

	if(drop >= 60)				//Between 60 - 100
		droppedItem = lookupTable.at(enemyType).second.at(0);		//Most Common item drop

	else if(drop >= 30)			//Between 30 - 59
		droppedItem = lookupTable.at(enemyType).second.at(1);		//2nd most common item drop

	else if(drop >= 15)			//Between 15 - 29
		droppedItem = lookupTable.at(enemyType).second.at(2);		//3rd most common item drop

	else if(drop >= 5)			//Between 5 - 14
		droppedItem = lookupTable.at(enemyType).second.at(3);		//4th most common item drop

	else															//Rarest item drop
		droppedItem = lookupTable.at(enemyType).second.at(4);
	
	return droppedItem;
}
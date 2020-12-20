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
	float random_integer = (float)GetRandomValue(-15, 15);
	float random_integer2 = (float)GetRandomValue(-15, 15);

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
	mHealth -= dmg;
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

int Enemy::GetRandomValue(int min, int max)
{
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased

	return uni(rng);
}

const std::string Enemy::GetDropItem()
{

	
	int max = 536870912;
	int drop = GetRandomValue(0, max);
	///Could add money to be dropped if a currency is going to be in the game

	InventoryUnordered::const_iterator retIt = --mpDropItems->end();

	bool found = false;

	InventoryUnordered::const_iterator it = mpDropItems->begin();
	while (it != mpDropItems->end() && !found)
	{
		max = max >> 1; // divides by 2
		found = true;
		if (drop >= max)
		{
			retIt = it;
		}
		++it;
	}

	// catch invalid iterator
	if (retIt == mpDropItems->end())
	{
		assert(false);
		retIt = --mpDropItems->end();
	}

	return (*retIt).first;

}
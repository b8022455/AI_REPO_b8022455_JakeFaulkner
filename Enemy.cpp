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

	int attackDuration = 2;		//How long the attack plays for, not final time yet
	int attackDelay = 4;		//How long between each attack

	times.StartTime(attackDuration, attackDelay);

	//Sets up collision point for attack
	attackCollisionPoint = mpInstance->World;
	//attackCollisionPoint._41 += 0.2f;

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
		x = -2.0f;
		break;

	case 1:										//Right
		x = 2.0f;
		break;

	case 2:										//Up
		z = 2.0f;
		break;

	case 3:										//Down
		z = -2.0f;
		break;
	}

	newPosition.x = x;
	newPosition.z = z;
}

void Enemy::ApplyBounceback()
{
	DirectX::XMFLOAT3 currentPos = this->GetPos();
	SetPos(DirectX::XMFLOAT3(currentPos.x + newPosition.x, currentPos.y, currentPos.z + newPosition.z));
	newPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
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

	InventoryUnordered::const_iterator retIt = mpDropItems->begin();

	bool found = false;

	InventoryUnordered::const_iterator it = mpDropItems->begin();
	while (it != mpDropItems->end() && !found)
	{
		max = max >> 1; // divides by 2
		if (drop >= max)
		{
			retIt = it;
			found = true;
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

void Enemy::StartAttack()
{
	//Particle based attack that does damage in front of enemy, has a timer for about 4 secs maybe
}

void Enemy::Update(const GameTimer& gt)
{
	//Follow Player
	if (times.isAttacking)
	{
		//Function to update attack animation (sets isAttacking to false when done)
		UpdateAttack();

		//Update the collision point
		attackCollisionPoint = mpInstance->World;
		attackCollisionPoint._41 -= 1.0f;		//AoE Attack
	}
	else
	{
		if (times.CanAttack())
		{
			StartAttack();
			times.SetNextTimer();		//Makes attacking bool true and resets timer for next attack

			attackCollisionPoint = mpInstance->World;
			attackCollisionPoint._41 -= 1.0f;
		}
		else
		{
			attackCollisionPoint = mpInstance->World;		//Keeps enemy collision point at enemy rather than in front of enemy
		}
	}

	times.UpdateTime();
}

void Enemy::UpdateAttack()
{
	if (times.currentTime.tm_sec > (times.nextAtkTime - times.attackDuration))
	{
		times.isAttacking = false;		//Attack has ended
	}
	//else
	//{
	//	//Do Attack animation
	//}
}

DirectX::XMFLOAT3 Enemy::GetCollisionPoint()
{
	return DirectX::XMFLOAT3(attackCollisionPoint._41,
							 attackCollisionPoint._42,
							 attackCollisionPoint._43);
}

void Enemy::SetDirection(int dir)
{
	playerDirection = dir;
}
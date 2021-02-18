#include "Enemy.h"
#include "GameApp.h"
// (NOTE) ENEMY LOOKS AT PLAYER IS IN GAMEOBJECT CLASS RATHER THAN HERE

void Enemy::SetHealth(int health) {
	mHealth = health;
}

void Enemy::MoveOffScreen()
{
	//Removes enemy
	this->mpInstance->World._42 = -200.f;

	//Removes particles
	for (auto& p : particles)
		p.mpInstance->World._42 = -200.0f;
}

void Enemy::Reset()
{
	// TODO: IMPLEMENT SPECIFIC HEALTH FOR EACH ENEMY
	SetRandomPosition();

	if (GetType() == GC::ENEMY_TYPE_1) {
		mHealth = GC::ENEMYTYPE1_HEALTH;
		BouncebackPosition = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
		times.isAttacking = false;
		for (auto& p : particles)
			p.RemoveEffect();
		times.UpdateTime();
		if (times.CanAttack())
			times.SetNextTimer();
	}
	if (GetType() == GC::ENEMY_TYPE_2) {
		mHealth = GC::ENEMYTYPE2_HEALTH;
		BouncebackPosition = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	}
}

//void Enemy::SetPosition(const DirectX::XMFLOAT3& newPosition)
//{
	//Updates position on the object
//	mpInstance->World._41 = newPosition.x;
//	mpInstance->World._42 = newPosition.y;
//	mpInstance->World._43 = newPosition.z;
//
//	//int attackDuration = 2;		//How long the attack plays for, not final time yet
//	//int attackDelay = 4;		//How long between each attack
//	
//	// ATTACK DELAY & DURATION HELD IN CONSTANTS
//	if (this->GetType() == GC::ENEMY_TYPE_1)
//		times.StartTime(GC::ENEMYTYPE1_ATTACK_DURATION, GC::ENEMYTYPE1_ATTACK_DELAY);
//
//	//Setup the enemy particles
//	particles.resize(20);
//
//}

void Enemy::SetRandomPosition()
{
	float random_integer = (float)GetRandomValue(0, 10) + 2.f;
	float random_integer2 = (float)GetRandomValue(0, 10) + 2.f;

	mpInstance->World._41 = random_integer;
	mpInstance->World._42 = 1.f;
	mpInstance->World._43 = random_integer2;
}

void Enemy::DamageEnemy(int dmg)
{
	mHealth -= dmg;

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

	BouncebackPosition.x = x;
	BouncebackPosition.z = z;
}

void Enemy::SetVelocity(const DirectX::SimpleMath::Vector3 target, const GameTimer& gt)
{

	DirectX::SimpleMath::Vector3 currentPos = GetPos();
	DirectX::SimpleMath::Vector3 direction = target - GetPos(); // AB = B-A
	direction.Normalize(); // 
	direction.y = 0; // xz plane
	direction *= mSpeed * gt.DeltaTime(); // apply speed

	mVelocity = direction;

	if (WithinBounds(currentPos + mVelocity))
		this->SetPos(currentPos + mVelocity);
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

void Enemy::Update(const GameTimer& gt) // TODO: (REMEMBER) IMPLEMENT LOGIC FOR EACH POTENTIAL AI BASED ON ENEMY TYPE
{
	if (mSpeed <= 0.0f)
		mSpeed = 0.0f;

	// BARFING ENEMY - SLOW MOVEMENT TOWARDS PLAYER WHEN NOT ATTACKING
	if (mEnemyType == GC::ENEMY_TYPE_1)
	{

		switch (mBehaviour)
		{
		case NONE:
			if (mSpeed > 0.0f)
			{
				mSpeed -= (GC::ENEMYTYPE1_DRAG * gt.DeltaTime());
			}
			; break;
		case CHASE:
			if (mSpeed < GC::ENEMYTYPE1_MAXSPEED)
			{
				mSpeed += (GC::ENEMYTYPE1_DRAG * gt.DeltaTime());
			}
			if (mSpeed >= GC::ENEMYTYPE1_MAXSPEED)
				mSpeed = GC::ENEMYTYPE1_MAXSPEED;
			; break;
		default:; break;
		}

		if (times.isAttacking) // EXECUTES ATTACK
			UpdateAttack(gt.DeltaTime());

		else
			if (times.EnemyCanAttack())
				//times.SetNextTimer();		//Makes attacking bool true and resets timer for next attack
				times.ResetTimer();

		// send int in instead of using gettype in the void
		times.EnemyUpdateTime(1, gt.DeltaTime());

		//Update enemy position based on bounceback
		if (BouncebackPosition.x != 0.0f || BouncebackPosition.z != 0.0f)		//If there was a bounceback
		{
			DirectX::XMFLOAT3 currentPos = this->GetPos();		//Get current position of player
			SetPos(DirectX::XMFLOAT3(currentPos.x + BouncebackPosition.x, 0.0f, currentPos.z + BouncebackPosition.z));		//Add the bounceback position to it, will be 0 if there is a collision
			BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
	}

	if (mEnemyType == GC::ENEMY_TYPE_2) // CHARGER ENEMY
	{
		switch (mBehaviour)
		{
		case NONE:
			if (mSpeed > 0.0f) // if speed is still being applied
			{
				mSpeed -= (GC::ENEMYTYPE2_DRAG * gt.DeltaTime()); // apply drag * deltatime to reduce
			}
			; break;
		case CHASE:
			if (mSpeed < GC::ENEMYTYPE2_MAXSPEED) // if speed is below max speed
			{
				mSpeed += (GC::ENEMYTYPE2_DRAG * gt.DeltaTime()); // add drag * deltatime to increase speed
			}
			if (mSpeed >= GC::ENEMYTYPE2_MAXSPEED)
				mSpeed = GC::ENEMYTYPE2_MAXSPEED;
			; break;
		default:; break;
		}

		//Update enemy position based on bounceback
		if (BouncebackPosition.x != 0.0f || BouncebackPosition.z != 0.0f)		//If there was a bounceback
		{
			DirectX::XMFLOAT3 currentPos = this->GetPos();		//Get current position of player
			SetPos(DirectX::XMFLOAT3(currentPos.x + BouncebackPosition.x, 0.0f, currentPos.z + BouncebackPosition.z));		//Add the bounceback position to it, will be 0 if there is a collision
			BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
	}

	mpInstance->World._42 = 0.0f;
}

void Enemy::UpdateAttack(float dt)
{
	// IF DURATION IS LESS THAN 0 SHOULD BE HERE
	times.attackDuration -= dt;

	if (times.attackDuration <= 0.0f) // (NEXT ATTACK TIMER - DURATION)
	{

		times.isAttacking = false;		//Attack has ended
		for (auto& p : particles)
			p.RemoveEffect();
		times.attackDuration = times.AtkDur;
	}
	else
	{
		//Get the rotation of the enemy currently
		float angle = GetRotationY();
		angle += 90;		//Adjust the angle to the enemys facing rotation

		//Do Attack animation
		for (auto& p : particles)
			p.Effect(GetPos(), angle);
	}
}

void Enemy::SetDirection(int dir) // may be enemy rotation?
{
	playerDirection = dir;
}
#pragma once
#include <ctime>

struct AttackTimeDelay		//All variables used in the timing stuff, kept in struct to keep them close together
{
	time_t timeAtNow;
	struct tm currentTime;
	float nextAtkTime = 0.0f; // time until next attack, set to attack delay when below 0 (use value)
	float AttackDelay = 0.0f; // time between attacks (reset value)
	float AtkDur = 0.0f;
	float attackDuration = 0.0f; // duration of an attack (reset value)
	errno_t storeLocaltime;
	bool isAttacking = false;
	int InvincibleDelay = 2;
	int nextInvincibleDelay = 0;
	bool IsInvincible = false; 

	float timer; // 

	void SetInvincibleStatus(bool foo)
	{
		this->IsInvincible = foo; 
	}

	void StartTime(float attDuration, float attDelay)
	{
		this->timeAtNow = time(0);
		this->nextAtkTime = 0;
		this->storeLocaltime = localtime_s(&this->currentTime, &this->timeAtNow);
		this->attackDuration = attDuration;
		this->AtkDur = attDuration;
		this->AttackDelay = attDelay;
	};

	void UpdateTime()
	{
		this->timeAtNow = time(0);
		this->storeLocaltime = localtime_s(&this->currentTime, &this->timeAtNow);
		if (this->currentTime.tm_sec < 2)	this->nextAtkTime = 0;
		if (this->nextAtkTime >= 60)	this->nextAtkTime = 2;
	};

	void EnemyUpdateTime(int e, float dt) // NEW ONE ADDED - decrease next atk time and reset it, if not attacking
	{
		if (this->nextAtkTime <= 0 && isAttacking == false) { 
			/*if (e == 1) this->nextAtkTime = GC::ENEMYTYPE1_ATTACK_DELAY;*/
			this->nextAtkTime = AttackDelay;
		}
		this->nextAtkTime -= dt;
	};

	bool CanAttack()
	{
		return this->currentTime.tm_sec > this->nextAtkTime;
	}

	bool EnemyCanAttack() { // NEW ONE ADDED - if the next atk time is below 0
		return this->nextAtkTime <= 0.0f;
	}

	void SetNextTimer()
	{
		this->nextAtkTime = this->currentTime.tm_sec + this->AttackDelay;		//Resets timer on attack delay
		this->isAttacking = true;	//Sets the state of the object to true
	}

	void ResetTimer() { // NEW ONE ADDED
		this->nextAtkTime = this->AttackDelay;
		this->isAttacking = true;
	}

};
#pragma once
#include <ctime>

struct AttackTimeDelay		//All variables used in the timing stuff, kept in struct to keep them close together
{
	time_t timeAtNow;
	struct tm currentTime;
	int nextAtkTime;
	int AttackDelay;
	int attackDuration;
	errno_t storeLocaltime;
	bool isAttacking = false;
	int InvincibleDelay = 2;
	int nextInvincibleDelay;
	bool IsInvincible = false;

	void SetInvincibleStatus(bool foo)
	{
		this->IsInvincible = foo;
	}

	void StartTime(int attDuration, int attDelay)
	{
		this->timeAtNow = time(0);
		this->nextAtkTime = 0;
		this->storeLocaltime = localtime_s(&this->currentTime, &this->timeAtNow);
		this->attackDuration = attDuration;
		this->AttackDelay = attDelay;
	};

	void UpdateTime()
	{
		this->timeAtNow = time(0);
		this->storeLocaltime = localtime_s(&this->currentTime, &this->timeAtNow);
		if (this->currentTime.tm_sec < 2)	this->nextAtkTime = 0;
		if (this->nextAtkTime >= 60)	this->nextAtkTime = 4;
	};

	// TODO: MAY NEED SPECIFIC ENEMY EQUIVALENT OF THE ABOVE IF MULTIPLE ENEMY TYPES IMPLEMENTED
	void EnemyUpdateTime(int e)
	{
		this->timeAtNow = time(0);
		this->storeLocaltime = localtime_s(&this->currentTime, &this->timeAtNow);
		if (this->currentTime.tm_sec < 2)	this->nextAtkTime = 0;
		if (this->nextAtkTime >= 60) { if (e == 1)this->nextAtkTime = GC::ENEMYTYPE1_ATTACK_DELAY; }
	};

	bool CanAttack()
	{
		return this->currentTime.tm_sec > this->nextAtkTime;
	}

	void SetNextTimer()
	{
		this->nextAtkTime = this->currentTime.tm_sec + this->AttackDelay;		//Resets timer on attack delay
		this->isAttacking = true;	//Sets the state of the object to true
	}

};
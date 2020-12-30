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
		if (this->currentTime.tm_sec == 0)	this->nextAtkTime = 0;
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
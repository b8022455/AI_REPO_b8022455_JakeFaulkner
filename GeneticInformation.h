#pragma once

class GeneticInformation
{
public:
	GeneticInformation(int health, float attackDelay, float movementSpeed, std::string type)
		: mHealth(health), mAttackDelay(attackDelay), mMovementSpeed(movementSpeed), mEnemyType(type)		//Inherits values from parents
	{};

	GeneticInformation()	{};

	//Used to get the initial populations genetics
	//void GetRandomGenetics()
	//{
	//	//Uses a random number generator to get initial values, value boundaries aren't final
	//	this->mHealth = GetRandomInt(20, 100);
	//	this->mAttackDelay = GetRandomFloat(0.2f, 1.0f);
	//	this->mMovementSpeed = GetRandomFloat(0.2f, 1.0f);
	//	this->mEnemyType = GetRandomBehaviour();
	//};

	//std::string GetEnemyType() { return mEnemyType; }
	//int GetHealth() { return mHealth; }

	//#pragma region Random Value Generator functions (GetRandomInt, GetRandomFloat, GetRandomBehaviour)

	//int GetRandomInt(int min, int max)
	//{
	//	std::random_device rd;
	//	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	//	std::uniform_int_distribution<int> uni(min, max);

	//	return uni(rng);
	//};

	//float GetRandomFloat(float min, float max)
	//{
	//	std::random_device rd;
	//	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	//	std::uniform_int_distribution<int> uni(static_cast<int>(min), static_cast<int>(max));

	//	return static_cast<float>(uni(rng));
	//}

	//std::string GetRandomBehaviour()
	//{
	//	int rand = GetRandomInt(0, 1);

	//	if (rand == 0)
	//		return "EnemyType1";
	//	else
	//		return "EnemyType2";
	//}

	//#pragma endregion

private:
	int mHealth;
	float mAttackDelay;		//Could make these ints instead?
	float mMovementSpeed;
	std::string mEnemyType;
};
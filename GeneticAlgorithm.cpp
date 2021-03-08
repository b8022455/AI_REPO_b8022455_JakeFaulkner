#include "GeneticAlgorithm.h"

void GeneticAlgorithm::CreateInitialPopulation()
{
	//Constructs enemies with random genetics to start
	for (int i = 0; i < EnemyCount; i++)
	{
		mPopulation.push_back(GetInitialCandidate());
	}
}

bool GeneticAlgorithm::CurrentGenerationFinished()
{
	return mPopulation.size() == 0;
}

void GeneticAlgorithm::MoveEnemyFromPopulation(const int offset)
{
	mDefeatedEnemies.push_back(mPopulation.at(offset));		//Add enemy to be checked during the mating process
	mPopulation.erase(mPopulation.begin() + offset);		//Remove from population
}

void GeneticAlgorithm::IncrementFitnessValue(int offset)
{
	mPopulation.at(offset).IncrementFitnessValue();
}

void GeneticAlgorithm::SelectCandidates()
{
	//Organises by fitness value of each enemy
	std::sort(mDefeatedEnemies.begin(), mDefeatedEnemies.end(), std::ref(organiseVector));

	ElitistSelection();		//Puts best candidate from population into next generation vector

	MatingProcess();		//Has current population mate, produces 4 children to add to the next generation vector

	GenerationTransition();	//Updates Enemy vectors to reflect new generation
}

void GeneticAlgorithm::ElitistSelection()
{
	mDefeatedEnemies.at(0).Reset();		//Resets health, fitness value to be used again
	mNextGeneration.push_back(mDefeatedEnemies.at(0));	//Adds to the next generation
}

void GeneticAlgorithm::MatingProcess()
{
	//Mate candidates from population (1st->3rd, 2nd->4th, 5th is discarded)
	for (int i = 0; i < 2; i++)
	{
		int j = i + 2;

		//Parents have 2 children to keep candidate count at 5 (2 children per couple, 1 from elite selection)
		mNextGeneration.push_back(Enemy(mDefeatedEnemies.at(i), mDefeatedEnemies.at(j)));
		mNextGeneration.push_back(Enemy(mDefeatedEnemies.at(i), mDefeatedEnemies.at(j)));
	}
}

void GeneticAlgorithm::GenerationTransition()
{
	//Make next population of candidates the next generation
	mPopulation = mNextGeneration;

	//Clear for future generations
	mNextGeneration.clear();
	mDefeatedEnemies.clear();
}

Enemy GeneticAlgorithm::GetInitialCandidate()
{
	Enemy Candidate;
	std::vector<int> geneticInformation;		//Is passed to the enemy for their genetic information

	//Create the randomized genetics to be given to initial enemy
	geneticInformation.push_back(GetRandomInt(10, 45));		//Health
	geneticInformation.push_back(GetRandomInt(10, 100));	//Attack Delay
	geneticInformation.push_back(GetRandomInt(20, 80));		//Movement Speed
	geneticInformation.push_back(GetRandomInt(10, 100));	//Enemy Sight Distance
	geneticInformation.push_back(GetRandomInt(1, 2));		//Behaviour Type
	geneticInformation.push_back(GetRandomInt(10, 100));	//Attack Duration

	Candidate.GetInitialGenetics(geneticInformation);		//Passes randomized genetics to enemy

	return Candidate;
}

int GeneticAlgorithm::GetRandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(min, max);

	return uni(rng);
}
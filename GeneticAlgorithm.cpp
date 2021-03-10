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
		mNextGeneration.push_back(GetOffspringCandidate(mDefeatedEnemies.at(i), mDefeatedEnemies.at(j)));
		mNextGeneration.push_back(GetOffspringCandidate(mDefeatedEnemies.at(i), mDefeatedEnemies.at(j)));
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
	geneticInformation.push_back(GetRandomInt(20, 100));	//Movement Speed
	geneticInformation.push_back(GetRandomInt(10, 100));	//Enemy Sight Distance
	geneticInformation.push_back(GetRandomInt(1, 2));		//Behaviour Type
	geneticInformation.push_back(GetRandomInt(10, 100));	//Attack Duration

	Candidate.GetInitialGenetics(geneticInformation);		//Passes randomized genetics to enemy

	return Candidate;
}

Enemy GeneticAlgorithm::GetOffspringCandidate(Enemy& parent1, Enemy& parent2)
{
	//Create local vector of genetics, that is passed into the enemy class
	Enemy offspring;
	std::vector<int> offspringGenetics;		//Is passed to the enemy for their genetic information

	//Get parent genetics
 	std::vector<int> parent1Genetics = parent1.GetChromosomes();
	std::vector<int> parent2Genetics = parent2.GetChromosomes();

	//Loops through each part of the genetics
	for (int i = 0; i < parent1Genetics.size(); i++)
	{
		int probability = GetRandomInt(0, 100);		//Get random probability whether to inherit from 1st parent (45%), 2nd parent (45%) or mutate (10%)

		if (probability < 45)						//Inherit from 1st parent
			offspringGenetics.push_back(parent1Genetics.at(i));

		else if (probability < 90)					//Inherit from 2nd parent
			offspringGenetics.push_back(parent2Genetics.at(i));

		else
			offspringGenetics.push_back(MutateProcess());		//Gets new random mutated value
	}

	//Prevents GameObject errors when instanciating enemy models
	if (offspringGenetics.at(4) > 2)
		offspringGenetics.at(4) = GetRandomInt(1, 2);

	//Passes inherited genetics to enemy
	offspring.GetInheritedGenetics(offspringGenetics);

	return offspring;
}

int GeneticAlgorithm::MutateProcess()
{
	return GetRandomInt(20, 100);
}

int GeneticAlgorithm::GetRandomInt(int min, int max)
{
	std::random_device rd;
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(min, max);

	return uni(rng);
}
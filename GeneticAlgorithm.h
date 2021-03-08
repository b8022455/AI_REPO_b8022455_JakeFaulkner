#pragma once
#include "Enemy.h"
#include <algorithm>

class GeneticAlgorithm
{
public:
	GeneticAlgorithm() {};		//Default Constructor

	void CreateInitialPopulation();		//Adds first generation of candidates to Enemy vectors
	void IncrementFitnessValue(int vectorOffset);		//Generically increments the fitness value of an enemy when they hit the player
	bool CurrentGenerationFinished();	//Returns if all candidates of the current generation have been defeated
	void SelectCandidates();	//Chooses which candidates are used for elite selection and mating processes

	void MoveEnemyFromPopulation(const int vectorOffset);

	std::vector<Enemy> mPopulation;			//Stores current candidates
private:

	void ElitistSelection();	//Chooses the best candidate from the population to advance to the next generation
	void MatingProcess();		//Has candidates of the current generation mate to produce offspring for next generation
	void GenerationTransition();			//Removes the previous generation and replaces with the next generation

	struct SortByFitness
	{
		bool operator()(Enemy& a, Enemy& b) { return a.GetFitnessValue() > b.GetFitnessValue(); }		//Sorts vector of enemies via fitness value, highest to lowest
	};

	SortByFitness organiseVector;
	std::vector<Enemy> mDefeatedEnemies;	//Stores candidates from current population when defeated
	std::vector<Enemy> mNextGeneration;		//Stores next population which is put into mPopulation
};
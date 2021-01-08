#pragma once


#include "GameObject.h"

class GameTimer;

/*
	Plant -> Grow -> Harvert -> Grow -> Harvest ...
	                         -> Dead
*/


class Plant : public GameObject
{
	enum Stage{DEAD, GROW,HARVEST}; // grow increases size. harvest caps scale at 1
	Stage mStage = Stage::GROW;
	float growRate = 0.1f;
	float regrowChance = 0.5f;
	float scale = 0.1f;
	float color = 1.0f; // flashes when harvestable
public:
	// apply scale/growth
	void Update(const GameTimer& gt);
	void Harvest();


};


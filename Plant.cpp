#include <stdlib.h>  
#include <cmath>
#include "Plant.h"
#include "GameApp.h"

void Plant::Update(const GameTimer & gt)
{
	switch (mStage)
	{
	case GROW:
		scale *= growRate * gt.DeltaTime(); 
		SetScale(scale);
		if (scale <= 1.0f)
			mStage = Stage::HARVEST;
		break;
	case HARVEST:
		color = sinf(gt.TotalTime());
		break;
	case DEAD:
		break;
	}
}

void Plant::Harvest()
{
	if (mStage == Stage::HARVEST)
	{
		const float outcome = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // 0.0f - 1.0f

		scale = 0.1f;
		SetScale(scale);
		if (outcome <= regrowChance)
		{
			mStage = Stage::GROW;
			color = 1.0f;
		}
		else
		{
			mStage = Stage::DEAD;
			color = 0.5f;
		}
	}
}
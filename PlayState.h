#pragma once
#include "State.h"
//#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"



class PlayState : public State
{
	GameObject mGameObject;
	Player mPlayer;
	Enemy mEnemy;

	virtual void Initialize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
};


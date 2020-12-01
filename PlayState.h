#pragma once
#include "State.h"
#include "GameObject.h"



class PlayState : public State
{
	GameObject mGameObject;

	virtual void Initialize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
};


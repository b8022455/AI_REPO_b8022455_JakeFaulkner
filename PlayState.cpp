#include "PlayState.h"

void PlayState::Initialize()
{
	mPlayer.Initialize("Player"); // todo adapt GameApp mPlayer to this state
	mGameObject.Initialize("Weapon");
	mEnemy.Initialize("Enemy");

}

void PlayState::Update(const GameTimer & gt)
{

	mGameObject.Update(gt);

}

void PlayState::Draw(const GameTimer & gt)
{
	mGameObject.Update(gt);

}

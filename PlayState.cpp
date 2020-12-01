#include "PlayState.h"

void PlayState::Initialize()
{
	mGameObject.Initialize("Player");



}

void PlayState::Update(const GameTimer & gt)
{

	mGameObject.Update(gt);

}

void PlayState::Draw(const GameTimer & gt)
{
	mGameObject.Update(gt);

}

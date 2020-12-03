#include "State.h"
#include <assert.h>
#include "GameApp.h"
#include "PlayState.h"

bool StateManager::IsValidState(const std::string stateName)
{
	return mStates.count(stateName) == 1;
}
void StateManager::Init()
{
	AddState("foo", std::make_unique<PlayState>());


	
	std::for_each(mStates.begin(), mStates.end(), [](auto& s) { s.second->Initialize(); });

}
void StateManager::Update(const GameTimer & gt)
{
	auto test = GameApp::Get().AspectRatio(); //todo remove. Example of accessing GameApp

	if (IsValidState(mCurrentState))
	{
		mStates[mCurrentState]->Update(gt);
	}
	else
	{
		assert(false);
	}

}
//void StateManager::Draw(const GameTimer& gt)
//{
//	if (IsValidState(mCurrentState))
//	{
//		mStates[mCurrentState]->Draw(gt);
//	}
//	else
//	{
//		assert(false);
//	}
//}
void StateManager::AddState(const std::string & name, std::unique_ptr<State> newState)
{
	//Check the a state of the same name doesn't exist
	if (mStates.count(name) == 0)
	{
		//Defaults to first state 
		if (mStates.size() == 0)
			mCurrentState = name;

		//Adds state to map
		mStates[name] = std::move(newState);
	}
	else
	{
		assert(false);
	}
}
void StateManager::RemoveState(const std::string & name)
{
	if (mCurrentState != name &&  IsValidState(name))
	{
		mStates.erase(name);
	}
	else
	{
		assert(false);
	}
}
void StateManager::ChangeState(const std::string & name)
{
	if (IsValidState(name))
	{
		mCurrentState = name;
	}
	else
	{
		assert(false);
	}
}

void StateManager::OnMouseDown(WPARAM btnState, int x, int y)
{
	if (IsValidState(mCurrentState))
		mStates[mCurrentState]->OnMouseDown(btnState,x,y);
	else
		assert(false);
}

void StateManager::OnMouseUp(WPARAM btnState, int x, int y)
{
	if (IsValidState(mCurrentState))
		mStates[mCurrentState]->OnMouseUp(btnState, x, y);
	else
		assert(false);
}

void StateManager::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (IsValidState(mCurrentState))
		mStates[mCurrentState]->OnMouseMove(btnState, x, y);
	else
		assert(false);
}

void StateManager::OnKeyboardInput(const GameTimer & gt)
{
	if (IsValidState(mCurrentState))
		mStates[mCurrentState]->OnKeyboardInput(gt);
	else
		assert(false);
}


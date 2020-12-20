#include "State.h"
#include <assert.h>
#include "GameApp.h"
#include "PlayState.h"
#include "MenuState.h"
#include "TradeState.h"

bool StateManager::IsValidState(const std::string stateName)
{
	return mStates.count(stateName) == 1;
}
void StateManager::Init()
{

	// Set up menu buttons
	Sprite buttonBg;
	buttonBg.textureSize.x = buttonBg.textureSize.x >> 1;
	buttonBg.textureSize.y = buttonBg.textureSize.y >> 1;
	buttonBg.sourceRectangle = { 0,0,(LONG)buttonBg.textureSize.x,(LONG)buttonBg.textureSize.y >> 2 };
	buttonBg.Initialise("uiTex",true);
	
	
	// Main menu
	Button btnW(buttonBg, "W Play", Button::Action::GOTO_GAME);
	Button btnA(buttonBg, "A Play", Button::Action::GOTO_GAME);
	Button btnD(buttonBg, "D Play", Button::Action::GOTO_GAME);
	Button btnS(buttonBg, "S Play", Button::Action::GOTO_GAME);
	AddState("MainMenu", std::make_unique<MenuState>(btnW, btnA, btnD, btnS));

	//PauseMenu
	btnW = Button(buttonBg, "W Resume", Button::Action::GOTO_GAME);
	btnA = Button(buttonBg, "A Resume", Button::Action::GOTO_GAME);
	btnD = Button(buttonBg, "D Resume", Button::Action::GOTO_GAME);
	btnS = Button(buttonBg, "S Resume", Button::Action::GOTO_GAME);
	AddState("PauseMenu", std::make_unique<MenuState>(btnW, btnA, btnD, btnS));

	// GameState
	AddState(GC::STATE_PLAY, std::make_unique<PlayState>());
	
	//Trade state
	AddState(GC::STATE_TRADE, std::make_unique<TradeState>());

	// Init all states
	std::for_each(mStates.begin(), mStates.end(), [](auto& s) { s.second->Initialize(); });

}
void StateManager::Update(const GameTimer & gt)
{
	auto test = GameApp::Get().AspectRatio(); //todo remove. Example of accessing GameApp

	//GameApp::Get().input.Update();

	GameApp::Get().mDebugLog << "State: \"" << mCurrentState << "\"\n";

	if (IsValidState(mCurrentState))
	{
		mStates[mCurrentState]->Update(gt);
	}
	else
	{
		assert(false);
	}

}
void StateManager::Draw(const GameTimer & gt)
{
	if (IsValidState(mCurrentState))
	{
		mStates[mCurrentState]->Draw(gt);
	}
	else
	{
		assert(false);
	}
}

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
		mStates[name]->OnResume();
	}
	else
	{
		assert(false);
	}
}

State * StateManager::GetState(const std::string & name)
{

	if (IsValidState(name))
	{
		return mStates[name].get();
	}
	else
	{
		// No such state
		assert(false);
		return nullptr;
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


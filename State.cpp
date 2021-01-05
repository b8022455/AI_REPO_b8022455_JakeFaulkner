#include "State.h"
#include <assert.h>
#include "GameApp.h"
#include "PlayState.h"
#include "MenuState.h"
#include "TradeState.h"
#include "GameOverState.h"

void StateManager::EvaluateState()
{
	if (mCurrentState == GC::STATE_PLAY || mCurrentState == GC::STATE_PAUSE || mCurrentState == GC::STATE_TRADE)
	{
		mIsMenu = false;
	}
	else // menus
	{
		mIsMenu = true;
	}
}

bool StateManager::IsValidState(const std::string stateName)
{
	return mStates.count(stateName) == 1;
}
void StateManager::Init()
{
	mMenuBackground.Initialise("iceTex");

	DirectX::XMINT2 s = GameApp::Get().GetClientSizeU2(); //todo on resize
	mMenuBackground.sourceRectangle = { 0,0,s.x,s.y };


	//set position of text elements of menus
	Text menuTitle, menuBody;
	menuTitle.position = GC::MENU_TITLE_POSITION;
	menuBody.position = GC::MENU_BODY_POSITION;

	// Set up menu buttons
	Sprite buttonBg;
	buttonBg.textureSize.x = buttonBg.textureSize.x >> 1;
	buttonBg.textureSize.y = buttonBg.textureSize.y >> 1;
	buttonBg.sourceRectangle = { 0,0,(LONG)buttonBg.textureSize.x,(LONG)buttonBg.textureSize.y >> 2 };
	buttonBg.destinationRectangle = { -1,-1,-1,-1 };
	buttonBg.Initialise("uiTex",true);
	
	
	// Main menu
	menuTitle.string = "Game Name";
	menuBody.string = "Placeholder text";
	Button btnW(buttonBg, "W Play", Button::Action::GOTO_GAME);
	Button btnA(buttonBg, "A Play", Button::Action::GOTO_GAME);
	Button btnD(buttonBg, "D Play", Button::Action::GOTO_GAME);
	Button btnS(buttonBg, "S Play", Button::Action::GOTO_GAME);
	AddState("MainMenu", std::make_unique<MenuState>(menuTitle, menuBody,btnW, btnA, btnD, btnS));

	//PauseMenu
	menuTitle.string = "Pause";
	menuBody.string = "Placeholder text";
	btnW = Button(buttonBg, "W Resume", Button::Action::GOTO_GAME);
	btnA = Button(buttonBg, "A Resume", Button::Action::GOTO_GAME);
	btnD = Button(buttonBg, "D Main Menu", Button::Action::GOTO_MAIN_MENU);
	btnS = Button(buttonBg, "S Resume", Button::Action::GOTO_GAME);
	AddState("PauseMenu", std::make_unique<MenuState>(menuTitle, menuBody,btnW, btnA, btnD, btnS));

	// GameState
	AddState(GC::STATE_PLAY, std::make_unique<PlayState>());
	
	//Trade state
	AddState(GC::STATE_TRADE, std::make_unique<TradeState>());

	//Game Over State
	Text GOTitle;
	GOTitle.scale = 1.5f;
	GOTitle.position = GC::MENU_TITLE_POSITION;
	GOTitle.string = "Game Over";

	Button btnRestart(buttonBg, "W Restart", Button::Action::GOTO_GAME);
	AddState(GC::STATE_GAMEOVER, std::make_unique<GameOverState>(GOTitle, btnRestart));


	// Init all states
	std::for_each(mStates.begin(), mStates.end(), [](auto& s) { s.second->Initialize(); });

	EvaluateState();

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
		if (IsMenu())
		{
			mMenuBackground.Draw();
		}

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
		EvaluateState();
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


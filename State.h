#pragma once


#include <vector>
#include <memory>
#include <map>
#include <string>

class GameTimer;

// Base class for all states
class State
{
public:
	virtual void Init() = 0;
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;
};

// State starting point
class StateA : public State
{
	int a = 7;
public:
	virtual void Init() override {};
	virtual void Update(const GameTimer& gt) override {};
	virtual void Draw(const GameTimer& gt) override {};
};

class StateManager
{
	// Holds states
	std::map<std::string, std::unique_ptr<State> > mStates;
	// Key to the state that will update and render
	std::string mCurrentState;
	// Validate state exists
	bool IsValidState(const std::string stateName);
public:
	void Init() {};
	void Update(const GameTimer& gt);
	void Draw(const GameTimer& gt);

	// Add unique_ptr of derived State class.
	void AddState(const std::string& name, std::unique_ptr<State> newState);
	// Cannot remove active state
	void RemoveState(const std::string& name);
	// Switch between states
	void ChangeState(const std::string& name);
};

// State starting point
class StateB : public State
{
	char b = 7;
public:
	virtual void Init() override {};
	virtual void Update(const GameTimer& gt) override {};
	virtual void Draw(const GameTimer& gt) override {};

	void DeleteThis();
};
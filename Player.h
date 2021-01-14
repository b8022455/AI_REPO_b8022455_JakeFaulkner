#pragma once

#include "Common/d3dApp.h"
#include "Common/GeometryGenerator.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "Common/Camera.h"
#include "GameObject.h"
#include "Velocity.h"
#include "DeltaTimer.h"
#include <d2d1helper.h>

//using namespace DirectX;

enum PlayerFacingDirection { Left, Right, Up, Down };			//Have the rotation of the players model replace this


class Player : public GameObject
{
public:

	void Update(const GameTimer& gt);
	void MoveUp(const GameTimer& gt);
	void MoveDown(const GameTimer& gt);
	void MoveLeft(const GameTimer& gt);
	void MoveRight(const GameTimer& gt);

	DirectX::XMFLOAT3 GetPositionWithVelocity(const GameTimer& gt);
	void SetVelocity(const float newVelocity);

	Velocity vel;
	// Gamepad
	void Move(const GameTimer& gt, const DirectX::SimpleMath::Vector3& vec);
	void DamagePlayer(int damage);

	int health = 100;

	float hazardTimer = 0; // needs to be set as needed, used to apply hazard effects (not every frame)
	bool Slowed = false;
	bool Slippy = false;

	// used to apply boundaries to player
	const float PLAYER_RIGHTBOUND = 15.0f;
	const float PLAYER_LEFTBOUND = -15.0f;

	const float PLAYER_UPBOUND = 15.0f;
	const float PLAYER_DOWNBOUND = -15.0f;

	PlayerFacingDirection playerDir = PlayerFacingDirection::Left;
	int attack = 10;		//Base attack stat (base amount + weapon amount), need to make private but public for debugging purposes

	DeltaTimer mFootstepTimer;

private:

	const float MAX_VELOCITY = 5.0f; // change as needed, replace move
	const float START_VELOCITY = 1.0f; // may not be needed
	// velocity increases with time as long as the button is held, velocity decreases to nothing when button not held
	// constantly apply velocity, but change when button is pressed
	float UP_velocity = 0.0f; //  increases when moving upwards up to max, decreases when not moving
	float DOWN_velocity = 0.0f; //  increases when moving downwards up to max, decreases when not moving
	float LEFT_velocity = 0.0f; //  increases when moving leftwards up to max, decreases when not moving
	float RIGHT_velocity = 0.0f; //  increases when moving rightwards up to max, decreases when not moving

	D2D1_RECT_F VELOCITY { 0.0f, //LEFT = left
	0.0f, //UP = top
	0.0f, //RIGHT = right
	0.0f //DOWN = bottom
	};

};
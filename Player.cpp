#include "Player.h"
#include "GameApp.h"

//#include "XmfloatOverload.h"

void Player::Update(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();
	GameApp::Get().mDebugLog << "Health:" << health << "\n";
	// APPLY VELOCITY HERE
	/*if (RIGHT_velocity >= 0)
		RIGHT_velocity -= dt * 8;
	if (RIGHT_velocity < 0)
		RIGHT_velocity = 0;
	if (LEFT_velocity >= 0)
		LEFT_velocity -= dt * 8;
	if (LEFT_velocity < 0)
		LEFT_velocity = 0;
	if (UP_velocity >= 0)
		UP_velocity -= dt * 8;
	if (UP_velocity < 0)
		UP_velocity = 0;
	if (DOWN_velocity >= 0)
		DOWN_velocity -= dt * 8;
	if (DOWN_velocity < 0)
		DOWN_velocity = 0;*/

	if (VELOCITY.right >= 0)
		VELOCITY.right -= dt * 8;
	if (VELOCITY.right < 0)
		VELOCITY.right = 0;
	if (VELOCITY.left >= 0)
		VELOCITY.left -= dt * 8;
	if (VELOCITY.left < 0)
		VELOCITY.left = 0;
	if (VELOCITY.top >= 0)
		VELOCITY.top -= dt * 8;
	if (VELOCITY.top < 0)
		VELOCITY.top = 0;
	if (VELOCITY.bottom >= 0)
		VELOCITY.bottom -= dt * 8;
	if (VELOCITY.bottom < 0)
		VELOCITY.bottom = 0;

	if (mpInstance->World._41 <= PLAYER_RIGHTBOUND) {
		if (Slowed == true)
			mpInstance->World._41 += (0.5f * VELOCITY.right/*RIGHT_velocity*/) * dt;
		if (Slippy == true)
			mpInstance->World._41 += (1.5f * VELOCITY.right/*RIGHT_velocity*/) * dt;
		if (Slowed != true && Slippy != true)
			mpInstance->World._41 += VELOCITY.right/*RIGHT_velocity*/ * dt;
	}
	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 -= (0.5f * VELOCITY.left/*LEFT_velocity*/) * dt;
		if (Slippy == true)
			mpInstance->World._41 -= (1.5f * VELOCITY.left/*LEFT_velocity*/) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._41 -= VELOCITY.left/*LEFT_velocity*/ * dt;
	}
	if (mpInstance->World._43 <= PLAYER_UPBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 += (0.5f * VELOCITY.top/*UP_velocity*/) * dt;
		if (Slippy == true)
			mpInstance->World._43 += (1.5f * VELOCITY.top/*UP_velocity*/) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._43 += VELOCITY.top/*UP_velocity*/ * dt;
	}
	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 -= (0.5f * VELOCITY.bottom/*DOWN_velocity*/) * dt;
		if (Slippy == true)
			mpInstance->World._43 -= (1.5f * VELOCITY.bottom/*DOWN_velocity*/) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._43 -= VELOCITY.bottom/*DOWN_velocity*/ * dt;
	}

	// if moving count down to next step
	if (VELOCITY.right/*RIGHT_velocity*/ > 0.1f || VELOCITY.left/*LEFT_velocity*/ > 0.1f || 
		VELOCITY.top/*UP_velocity*/ > 0.1f || VELOCITY.bottom/*DOWN_velocity*/ > 0.1f)
	{
		if (mFootstepTimer.HasTimeElapsed(gt.DeltaTime(), 0.5f))
		{
			GameApp::Get().GetAudio().Play("playerFootstep", nullptr, false, 1.0f, GC::FOOTSTEP_PITCH[rand() % GC::FOOTSTEP_PITCH_COUNT]);
		}
	}

	SimpleMath::Vector3 pos = GetPos();
	SimpleMath::Vector3 v = vel.GetVelocity();

	SetPos(pos + v);

}

void Player::MoveUp(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.top < MAX_VELOCITY)
		VELOCITY.top += dt * 12;

	// INCREASE VELOCITY HERE
	//if (UP_velocity < MAX_VELOCITY)
	//	UP_velocity += dt * 12; // add double time to velocity

	playerDir = PlayerFacingDirection::Up;
	SetRotationY(90);						//Positions player model facing upwards (away from camera)
}

void Player::MoveDown(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.bottom < MAX_VELOCITY)
		VELOCITY.bottom += dt * 12;

	// INCREASE VELOCITY HERE
	//if (DOWN_velocity < MAX_VELOCITY)
	//	DOWN_velocity += dt * 12; // add double time to velocity

	playerDir = PlayerFacingDirection::Down;
	SetRotationY(-90);							//Positions player model facing downwards (towards camera)
}

void Player::MoveLeft(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.left < MAX_VELOCITY)
		VELOCITY.left += dt * 12;

	// INCREASE VELOCITY HERE
	//if (LEFT_velocity < MAX_VELOCITY)
	//	LEFT_velocity += dt * 12; // add double time to velocity

	playerDir = PlayerFacingDirection::Left;
	SetRotationY(0);							//Positions player model facing left
}

void Player::MoveRight(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.right < MAX_VELOCITY)
		VELOCITY.right += dt * 12;

	// INCREASE VELOCITY HERE
	//if (RIGHT_velocity < MAX_VELOCITY)
	//	RIGHT_velocity += dt * 12; // add double time to velocity

	playerDir = PlayerFacingDirection::Right;
	SetRotationY(180);						//Positions player model facing right
}


void Player::Move(const GameTimer & gt, const DirectX::SimpleMath::Vector3 & vec)
{
	//Changes position of sword depending on where player is facing
	if (vec.x < 0)
	{
		playerDir = PlayerFacingDirection::Left;
		SetRotationY(0);							//Positions player model facing left
	}
	else if (vec.x > 0)
	{
		playerDir = PlayerFacingDirection::Right;
		SetRotationY(180);						//Positions player model facing right
	}

	if (vec.z > 0)
	{
		playerDir = PlayerFacingDirection::Up;
		SetRotationY(90);						//Positions player model facing upwards (away from camera)
	}
	else if (vec.z < 0)
	{
		playerDir = PlayerFacingDirection::Down;
		SetRotationY(-90);							//Positions player model facing downwards (towards camera)
	}

	//todo simplify

	vel.SetVel(vec, 1.0f*gt.DeltaTime());
}

void Player::DamagePlayer(int damage)			//When enemy hits with player
{
	float x = 0.0f;
	float z = 0.0f;
	health -= damage;

	switch (playerDir)
	{
	case 0:									//Left
		x = 2.5f;
		break;

	case 1:									//Right
		x = -2.5f;
		break;

	case 2:									//Up
		z = -2.5f;
		break;

	case 3:									//Down
		z = 2.5f;
		break;
	}

	mpInstance->World._41 += x;
	mpInstance->World._43 += z;
}

XMFLOAT3 Player::GetPositionWithVelocity(const GameTimer& gt)
{
	//Gets the 'potential' next position of the player before actually assigning it to the player position
	//Used to prevent player entering collision boxes
	float dt = gt.DeltaTime();
	XMFLOAT3 playerPosWithVelocity = this->GetPos();

	if (Input::Get().GamePadConnected())		//Why is there different velocity variables for keyboard and controller?
	{
		playerPosWithVelocity.x += vel.GetVelocity().x;
		playerPosWithVelocity.z += vel.GetVelocity().z;
	}
	else
	{
		playerPosWithVelocity.x += VELOCITY.right * dt;
		playerPosWithVelocity.x -= VELOCITY.left * dt;

		playerPosWithVelocity.z += VELOCITY.top * dt;
		playerPosWithVelocity.z -= VELOCITY.bottom * dt;
	}

	return playerPosWithVelocity;
}

void Player::SetVelocity(const float newVel)
{
	//Only change the velocity of the direction that caused the collision

	//Solution until velocity is vec2f
	VELOCITY.top = newVel;
	VELOCITY.bottom = newVel;
	VELOCITY.left = newVel;
	VELOCITY.right = newVel;

	const SimpleMath::Vector3 t = {0.0f, 0.0f, 0.0f};
	vel.SetVel(t, newVel);
}
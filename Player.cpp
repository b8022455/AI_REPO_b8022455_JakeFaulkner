#include "Player.h"
#include "GameApp.h"

//#include "XmfloatOverload.h"

void Player::Update(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();
	GameApp::Get().mDebugLog << "Health:" << health << "\n";
	// APPLY VELOCITY HERE
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
			mpInstance->World._41 += (0.5f * VELOCITY.right) * dt;
		if (Slippy == true)
			mpInstance->World._41 += (1.5f * VELOCITY.right) * dt;
		if (Slowed != true && Slippy != true)
			mpInstance->World._41 += VELOCITY.right * dt;
	}
	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 -= (0.5f * VELOCITY.left) * dt;
		if (Slippy == true)
			mpInstance->World._41 -= (1.5f * VELOCITY.left) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._41 -= VELOCITY.left * dt;
	}
	if (mpInstance->World._43 <= PLAYER_UPBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 += (0.5f * VELOCITY.top) * dt;
		if (Slippy == true)
			mpInstance->World._43 += (1.5f * VELOCITY.top) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._43 += VELOCITY.top * dt;
	}
	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 -= (0.5f * VELOCITY.bottom) * dt;
		if (Slippy == true)
			mpInstance->World._43 -= (1.5f * VELOCITY.bottom) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._43 -= VELOCITY.bottom * dt;
	}

	// if moving count down to next step
	if (VELOCITY.right > 0.1f || VELOCITY.left > 0.1f || VELOCITY.top > 0.1f || VELOCITY.bottom > 0.1f)
	{
		if (mFootstepTimer.HasTimeElapsed(gt.DeltaTime(), 0.5f))
		{
			GameApp::Get().GetAudio().Play("playerFootstep", nullptr, false, 1.0f, GC::FOOTSTEP_PITCH[rand() % GC::FOOTSTEP_PITCH_COUNT]);
		}
	}

}

void Player::MoveUp(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.top < MAX_VELOCITY)
		VELOCITY.top += dt * 12;

	playerDir = PlayerFacingDirection::Up;
	SetRotationY(0);						//Positions player model facing upwards (away from camera)
}

void Player::MoveDown(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.bottom < MAX_VELOCITY)
		VELOCITY.bottom += dt * 12;

	playerDir = PlayerFacingDirection::Down;
	SetRotationY(180);							//Positions player model facing downwards (towards camera)
}

void Player::MoveLeft(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.left < MAX_VELOCITY)
		VELOCITY.left += dt * 12;

	playerDir = PlayerFacingDirection::Left;
	SetRotationY(-90);							//Positions player model facing left
}

void Player::MoveRight(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.right < MAX_VELOCITY)
		VELOCITY.right += dt * 12;

	playerDir = PlayerFacingDirection::Right;
	SetRotationY(90);						//Positions player model facing right
}


void Player::Move(const GameTimer & gt, const DirectX::SimpleMath::Vector3 & vec)
{
	//Changes position of sword depending on where player is facing
	if (vec.x < 0)
	{
		//playerDir = PlayerFacingDirection::Left;
	}
	else if (vec.x > 0)
	{
		//playerDir = PlayerFacingDirection::Right;
	}

	if (vec.z > 0)
	{
		//playerDir = PlayerFacingDirection::Up;
	}
	else if (vec.z < 0)
	{
		//playerDir = PlayerFacingDirection::Down;
	}

	//todo simplify

	vel.SetVel(vec, 1.0f*gt.DeltaTime());
	SimpleMath::Vector3 pos = GetPos();
	SimpleMath::Vector3 v = vel.GetVelocity();

	SetPos(pos + v);
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
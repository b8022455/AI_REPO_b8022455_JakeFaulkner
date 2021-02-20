#include "Player.h"
#include "GameApp.h"

//#include "XmfloatOverload.h"

void Player::Update(const GameTimer & gt)
{
	const float dt = gt.DeltaTime();
	GameApp::Get().mDebugLog << "Health:" << health << "\n";
	

	//Check if player is invincible from recent hit
	times.UpdateTime();
	if (times.currentTime.tm_sec > times.nextInvincibleDelay)
		times.SetInvincibleStatus(false);

	if (BouncebackPosition.x != 0.0f || BouncebackPosition.z != 0.0f)		//If there has been a bounceback
	{
		DirectX::XMFLOAT3 currentPos = this->GetPos();
		SetPos(DirectX::XMFLOAT3(currentPos.x + BouncebackPosition.x, currentPos.y, currentPos.z + BouncebackPosition.z));
		BouncebackPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

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

	// LOAD NEXT AREA BY WALKING TO BORDER HERE - currently within 1 tile of border
	if (mpInstance->World._41 >= GC::WORLD_RIGHT_BOUNDARY - 0.9375f) { // 32 tiles = 30f world space, 1 tile = 0.9375f world space
		genArea = true;

	}
	if (mpInstance->World._41 <= GC::WORLD_LEFT_BOUNDARY + 0.9375f) { // 32 tiles = 30f world space, 1 tile = 0.9375f world space
		genArea = true;

	}
	if (mpInstance->World._43 >= GC::WORLD_TOP_BOUNDARY - 0.9375f) { // 32 tiles = 30f world space, 1 tile = 0.9375f world space
		genArea = true;

	}
	if (mpInstance->World._43 <= GC::WORLD_BOTTOM_BOUNDARY + 0.9375f) { // 32 tiles = 30f world space, 1 tile = 0.9375f world space
		genArea = true;

	}

	if (mpInstance->World._41 <= GC::WORLD_RIGHT_BOUNDARY) {
		if (Slowed == true)
			mpInstance->World._41 += (0.5f * VELOCITY.right) * dt;
		if (Slippy == true)
			mpInstance->World._41 += (1.5f * VELOCITY.right) * dt;
		if (Slowed != true && Slippy != true)
			mpInstance->World._41 += VELOCITY.right * dt;
	}
	if (mpInstance->World._41 >= GC::WORLD_LEFT_BOUNDARY)
	{
		if (Slowed == true)
			mpInstance->World._41 -= (0.5f * VELOCITY.left) * dt;
		if (Slippy == true)
			mpInstance->World._41 -= (1.5f * VELOCITY.left) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._41 -= VELOCITY.left * dt;
	}
	if (mpInstance->World._43 <= GC::WORLD_TOP_BOUNDARY)
	{
		if (Slowed == true)
			mpInstance->World._43 += (0.5f * VELOCITY.top) * dt;
		if (Slippy == true)
			mpInstance->World._43 += (1.5f * VELOCITY.top) * dt;
		if (Slowed == false && Slippy == false)
			mpInstance->World._43 += VELOCITY.top * dt;
	}
	if (mpInstance->World._43 >= GC::WORLD_BOTTOM_BOUNDARY)
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
		if (mAnimationTimer.HasTimeElapsed(gt.DeltaTime(), 0.5f))
		{
			if (scaleYValue == 0.8f)	scaleYValue = 1.f;				//Could find way to do alter between scales differently
			else if (scaleYValue == 1.f) scaleYValue = 0.8f;

			SetScaleY(scaleYValue);
		}

		Footsteps(gt);
	}

	SimpleMath::Vector3 pos = GetPos();
	SimpleMath::Vector3 v = vel.GetVelocity();

	SetPos(pos + v);

}

void Player::Reset()
{
	SetPos(DirectX::XMFLOAT3(0.f, 0.f, 0.f));		//Resets back to starting position
	SetRotationY(0);
	health = 100;
	SetVelocity(0);
	BouncebackPosition = DirectX::XMFLOAT3(0.f, 0.f, 0.f);		//Not sure if needs to be reset

	//Resetting attack delay
	times.UpdateTime();
	times.SetNextTimer();
	times.SetInvincibleStatus(false);

	playerDir = PlayerFacingDirection::Left;		//Find way to replace instead of resetting

	//Reset player bobbing animation
	scaleYValue = 1.f;
	SetScaleY(scaleYValue);
}

void Player::MoveUp(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.top < MAX_VELOCITY)
		VELOCITY.top += dt * 12;

	playerDir = PlayerFacingDirection::Up;
	SetRotationY(-90);						//Positions player model facing upwards (away from camera)
}

void Player::MoveDown(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.bottom < MAX_VELOCITY)
		VELOCITY.bottom += dt * 12;

	playerDir = PlayerFacingDirection::Down;
	SetRotationY(90);							//Positions player model facing downwards (towards camera)
}

void Player::MoveLeft(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.left < MAX_VELOCITY)
		VELOCITY.left += dt * 12;

	playerDir = PlayerFacingDirection::Left;
	SetRotationY(0);							//Positions player model facing left
}

void Player::MoveRight(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (VELOCITY.right < MAX_VELOCITY)
		VELOCITY.right += dt * 12;

	playerDir = PlayerFacingDirection::Right;
	SetRotationY(180);						//Positions player model facing right
}

void Player::Footsteps(const GameTimer& gt)
{
	if (mFootstepTimer.HasTimeElapsed(gt.DeltaTime(), 0.5f))
	{
		if (Slowed)
		{
			GameApp::Get().GetAudio().Play("playerFootstep", nullptr, false, 1.0f, GC::FOOTSTEP_PITCH[rand() % GC::FOOTSTEP_PITCH_COUNT]);
		}
		else if (Slippy)
		{
			GameApp::Get().GetAudio().Play("playerFootstepStone", nullptr, false, 1.0f, GC::FOOTSTEP_PITCH[rand() % GC::FOOTSTEP_PITCH_COUNT]);
		}
		else
		{
			GameApp::Get().GetAudio().Play("playerFootstepGrass", nullptr, false, 1.0f, GC::FOOTSTEP_PITCH[rand() % GC::FOOTSTEP_PITCH_COUNT]);
		}
	}
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
		SetRotationY(-90);						//Positions player model facing upwards (away from camera)
	}
	else if (vec.z < 0)
	{
		playerDir = PlayerFacingDirection::Down;
		SetRotationY(90);							//Positions player model facing downwards (towards camera)
	}

	//todo simplify

	vel.SetVel(vec, 1.0f*gt.DeltaTime());

	//Copied from Player::Move since keyboard/controller have diff functions to apply velocity
	if (vel.GetVelocity().x != 0.0f || vel.GetVelocity().z != 0.0f)
	{
		if (mAnimationTimer.HasTimeElapsed(gt.DeltaTime(), 0.5f))
		{
			if (scaleYValue == 0.8f)	scaleYValue = 1.f;				//Could find way to do alter between scales differently
			else if (scaleYValue == 1.f) scaleYValue = 0.8f;

			SetScaleY(scaleYValue);
		}

		Footsteps(gt);

	}
}

void Player::DamagePlayer(int damage)			//When enemy hits with player
{
	if (!times.IsInvincible)
	{
		health -= damage;
		if (health < 0)	health = 0;		//Prevents assert error in health bar (line 59 of bar.h)
		times.SetInvincibleStatus(true);
		times.nextInvincibleDelay = times.currentTime.tm_sec + times.InvincibleDelay;
		if (times.nextInvincibleDelay >= 60)	times.nextInvincibleDelay -= 60;
	}

	float x = 0.0f;
	float z = 0.0f;

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

	BouncebackPosition.x = x;
	BouncebackPosition.z = z;
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
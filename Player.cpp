#include "Player.h"
#include "GameApp.h"



void Player::Update(const GameTimer & gt)
{
	GameApp::Get().mDebugLog << "Health:" << health << "\n";
}

void Player::MoveUp(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (mpInstance->World._43 <= PLAYER_UPBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 += (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._43 += MOVE * dt;

		playerDir = PlayerFacingDirection::Up;
	}
}

void Player::MoveDown(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (mpInstance->World._43 >= PLAYER_DOWNBOUND)
	{
		if (Slowed == true)
			mpInstance->World._43 -= (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._43 -= MOVE * dt;

		playerDir = PlayerFacingDirection::Down;
	}

}

void Player::MoveLeft(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (mpInstance->World._41 >= PLAYER_LEFTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 -= (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._41 -= MOVE * dt;

		playerDir = PlayerFacingDirection::Left;
	}

}

void Player::MoveRight(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (mpInstance->World._41 <= PLAYER_RIGHTBOUND)
	{
		if (Slowed == true)
			mpInstance->World._41 += (0.5f * MOVE) * dt;
		if (Slowed == false)
			mpInstance->World._41 += MOVE * dt;

		playerDir = PlayerFacingDirection::Right;
	}

}

void Player::DamagePlayer(int damage)			//When enemy hits with player
{
	float x = 0.0f;
	float z = 0.0f;
	health -= damage;

	switch (playerDir)
	{
	case 0:									//Left
		x = 5.0f;
		break;

	case 1:									//Right
		x = -5.0f;
		break;

	case 2:									//Up
		z = -5.0f;
		break;

	case 3:									//Down
		z = 5.0f;
		break;
	}

	mpInstance->World._41 += x;
	mpInstance->World._43 += z;
}

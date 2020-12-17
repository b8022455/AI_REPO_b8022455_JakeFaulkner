#pragma once

#include <string>
#include <DirectXMath.h>
namespace GC
{
	//Keyboard keys

	const unsigned char 

		KEY_FW = 'W',
		KEY_BK = 'S',
		KEY_RT = 'D',
		KEY_LT = 'A',
		KEY_RAISE = 'E',
		KEY_LOWER = 'Q',

		KEY_INVENTORY = 'I',
	    KEY_USEITEM = 'B',


		KEY_ATTACK = 0x20,  //space
		KEY_PAUSE = 0xD, //enter
		KEY_CAM = 0x70, // F1

		KEY_DEBUG_ENEMY_POS = 'G'
		; 
	
	const int PLAYER_LOW_HEALTH = 50;


	const std::string
		GO_PLAYER = "Player",
		GO_ENEMY = "Enemy",
		GO_WEAPON = "Weapon",
		GO_TILE = "Tiles"

		;

	const DirectX::XMFLOAT3
		DIRECTION_FWD({ 0.0f,	0.0f,	1.0f }),
		DIRECTION_BCK({ 0.0f,	0.0f,	-1.0f }),
		DIRECTION_RGT({ 1.0f,	0.0f,	0.0f }),
		DIRECTION_LFT({ -1.0f,	0.0f,	0.0f }),
		DIRECTION_RAISE({ 0.0f,	1.0f,	0.0f }),
		DIRECTION_LOWER({ 0.0f,	-1.0f,	0.0f })
		;

}
#pragma once

#include <string>

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


		KEY_ATTACK = 0x20,  //space
		KEY_PAUSE = 0xD, //enter
		KEY_CAM = 0x70, // F1

		KEY_DEBUG_ENEMY_POS = 'G'
		; 
	
	const int PLAYER_LOW_HEALTH = 95;


	const std::string
		GO_PLAYER = "Player",
		GO_ENEMY = "Enemy",
		GO_WEAPON = "Weapon",
		GO_TILE = "Tiles"

		;

}
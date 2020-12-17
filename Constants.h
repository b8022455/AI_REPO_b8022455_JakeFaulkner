#pragma once

#include <string>
#include <DirectXMath.h>
#include "Item.h"
#include <array>
#include <vector>
#include <unordered_map>

typedef const std::vector<Item> ItemList;
typedef const std::vector<const Item*> ItemListPointers;

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

	const int
		EXP_EXPONENT = 3,
		EXP_OFFSET = 5,
		EXP_DEFAULT = 5; // test value for increasing exp

	const ItemList ITEM_LIST =
	{
		{"Empty"},
		{"Potion", ItemCategory::Healing, 2.5},
		{"Leadpipe", ItemCategory::Weapons, 3.5},
		{"Super Potion", ItemCategory::Healing, 5.0},
		{"Nail Bat", ItemCategory::Weapons, 10.0},
		{"Plastic Spork", ItemCategory::Weapons, 0.5},
		{"Holy Water", ItemCategory::Farming, 12.0},
		{"Magical Seeds", ItemCategory::Farming, 20.0},
		{"Key To Valhalla", ItemCategory::KeyItems, 0.0}
	};

	const ItemListPointers ITEMS_ENEMY_1{
		&ITEM_LIST[0],
		&ITEM_LIST[1],
		&ITEM_LIST[2],
		&ITEM_LIST[3],
	};

	const ItemListPointers ITEMS_ENEMY_2{
		&ITEM_LIST[0],
		&ITEM_LIST[4],
		&ITEM_LIST[5],
		&ITEM_LIST[6],
	};

	const ItemListPointers ITEMS_ENEMY_2{
		&ITEM_LIST[0],
		&ITEM_LIST[4],
		&ITEM_LIST[5],
		&ITEM_LIST[6],
	};

	const ItemListPointers ITEMS_ENEMY_BOSS{
		&ITEM_LIST[8],
	};




	
}
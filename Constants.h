#pragma once

#include <string>
#include <DirectXMath.h>
#include "Item.h"
#include <array>
#include <vector>
#include <map>

typedef const std::vector<Item> ItemList;
typedef const std::vector<const Item*> ItemListPointers;
typedef const std::vector<size_t> ItemLookups;



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
		GO_TILE = "Tiles",


		STATE_PLAY = "PlayState"

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



	const ItemList_v2 ITEM_LIST =
	{
		{"Potion",			{ItemCategory::Healing, 2.5}},
		{"Leadpipe",		{ItemCategory::Weapons, 3.5}},
		{"Super Potion",	{ItemCategory::Healing, 5.0}},
		{"Nail Bat",		{ItemCategory::Weapons, 10.0}},
		{"Plastic Spork",	{ItemCategory::Weapons, 0.5}},
		{"Holy Water",		{ItemCategory::Farming, 12.0}},
		{"Magical Seeds",	{ItemCategory::Farming, 20.0}},
		{"Key To Valhalla",	{ItemCategory::KeyItems, 0.0}}
	};


	const ItemLookup_v2 ITEM_LOOKUP_ENEMIES =
	{
		//enemy require 4 items
		{ "EnemyType1",			{ "Empty","Leadpipe","Nail Bat", "Magical Seeds" }   },
		{ "EnemyType2",			{ "Empty","Leadpipe","Holy Water", "Plastic Spork" }   },
		{ "EnemyTypeBoss",		{ "Key To Valhalla","Key To Valhalla","Key To Valhalla", "Key To Valhalla" }   },
	};

	//Trader requests. upto 3
	const ItemLookup_v2 ITEM_LOOKUP_REQUEST
	{
		{ "Traveller1",			{"Leadpipe" }   },
		{ "Traveller2",			{"Potion", "Nail Bat", "Holy Water"}   },
		{ "Traveller3",			{"Potion", "Nail Bat", "Holy Water"}   }, //todo change 
		{ "Traveller4",			{"Potion", "Nail Bat", "Holy Water"}   }, //todo change
	};

	const ItemLookup_v2 ITEM_LOOKUP_REWARD
	{
		{ "Traveller1",			{"Leadpipe" }							},
		{ "Traveller2",			{"Nail Bat", "Leadpipe", "Holy Water"}  }, //
		{ "Traveller3",			{"Nail Bat", "Nail Bat", "Holy Water"}  }, //todo change 
		{ "Traveller4",			{"Nail Bat", "Nail Bat", "Holy Water"}  }, //todo change
		{ "Traveller5",			{}										}, // no reward, charity
	};
		 
	const ItemListPointers ITEMS_ENEMY_2{
		&ITEM_LIST[0],
		&ITEM_LIST[4],
		&ITEM_LIST[5],
		&ITEM_LIST[6],
	};

	const ItemListPointers ITEMS_ENEMY_3{
		&ITEM_LIST[0],
		&ITEM_LIST[4],
		&ITEM_LIST[5],
		&ITEM_LIST[6],
	};

	const ItemListPointers ITEMS_ENEMY_BOSS{
		&ITEM_LIST[8],
	};

	const ItemListPointers ITEMS_TRADER_REQUEST_1{
		&ITEM_LIST[2],
	};

	const ItemListPointers ITEMS_TRADER_REWARD_1{
		&ITEM_LIST[1],
	};

	
}
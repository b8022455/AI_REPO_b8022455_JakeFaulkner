#pragma once

#include <string>
#include <DirectXMath.h>
#include "Item.h"
#include <array>
#include <vector>
#include <map>


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

		KET_TALK = 'T',


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


	const std::string
		TRADER_NAME_TEST	= "TravellerTest",
		TRADER_NAME_1		= "Traveller1",
		TRADER_NAME_2		= "Traveller2",
		TRADER_NAME_3		= "Traveller3",
		TRADER_NAME_4		= "Traveller4"
		;



	const ItemMap ITEM_LIST = {
		{"Empty",			{ItemCategory::None,	0}}, // 
		{"Radio",			{ItemCategory::KeyItems,  20}},
		{"Potion",			{ItemCategory::Healing,  20}},
		{"Leadpipe",		{ItemCategory::Weapons,  30}},
		{"Super Potion",	{ItemCategory::Healing,  50}},
		{"Nail Bat",		{ItemCategory::Weapons, 100}},
		{"Plastic Spork",	{ItemCategory::Weapons,  00}},
		{"Holy Water",		{ItemCategory::Farming, 120}},
		{"Magical Seeds",	{ItemCategory::Farming, 200}},
		{"Key To Valhalla",	{ItemCategory::KeyItems,  0}}
	};

	const ItemLookup ITEM_LOOKUP_ENEMIES =
	{
		{ "EnemyType1",			{ {"Empty",1},	{"Leadpipe",2},{"Nail Bat",2}, {"Magical Seeds",2}	}   },
		{ "EnemyType2",			{ {"Empty",1},	{"Holy Water",2},{"Potion",2}, {"Plastic Spork",2}	}   },
		{ "EnemyTypeBoss",		{ {"Key To Valhalla",1}												}   },
	};

	//Trader requests. upto 3
	const ItemLookup ITEM_LOOKUP_REQUEST
	{
		{ TRADER_NAME_TEST,			{{"Potion",1 }													}},
		{ TRADER_NAME_1,			{{"Leadpipe",2 }	}},
		{ TRADER_NAME_2,			{{"Potion",2}, {"Plastic Spork",2},{ "Holy Water",2}			}   },
		{ TRADER_NAME_3,			{{"Potion",2}, {"Nail Bat",2},{ "Leadpipe",2}					}   }, //todo change 
		{ TRADER_NAME_4,			{{"Potion",2}, {"Leadpipe",2},{ "Holy Water",2}					}   }, //todo change
	};

	const ItemLookup ITEM_LOOKUP_REWARD
	{
		{ TRADER_NAME_TEST,		{{"Key To Valhalla",1 }											}	},
		{ TRADER_NAME_1,		{{"Leadpipe",2}													}	},
		{ TRADER_NAME_2,		{{"Potion",2}, {"Leadpipe",2},{ "Holy Water",2}					}	}, //
		{ TRADER_NAME_3,		{{"Potion",2}, {"Holy Water",2},{ "Holy Water",2}				}	}, //todo change 
		{ TRADER_NAME_4,		{																}	}, // no reward, charity
	};
		 

	

	const int
		HEAL_SMALL = 10,
		HEAL_MED = 50,
		HEAL_LARGE = 100
		;
}
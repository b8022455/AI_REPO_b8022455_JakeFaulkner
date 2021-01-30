#pragma once

#include <string>
#include <DirectXMath.h>
#include "Item.h"
#include <array>
#include <vector>
#include <map>



typedef std::pair<std::string, std::string> DialogPair;

typedef std::pair<std::string, float> PlantData;

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
		KEY_USEITEM = 'U',

		KET_TALK = 'T',


		KEY_ATTACK = 0x20,  //space
		KEY_PAUSE = 0xD, //enter
		KEY_CAM = 0x70, // F1

		KEY_DEBUG_GENERATE = 'R', // used for map regeneration command

		KEY_DEBUG_ENEMY_POS = 'G'
		;

	const size_t
		NUM_DIFF_RENDER_OBJS = 6 // for frame resource
		;

	const int
		PLAYER_LOW_HEALTH = 50,

		EXP_EXPONENT = 3,
		EXP_OFFSET = 5,
		EXP_DEFAULT = 5, // test value for increasing exp

		SCORE_PLANT = 150,
		SCORE_ENEMY = 250,
		SCORE_LVLUP = 100,

		HEAL_SMALL = 10,
		HEAL_MED = 50,
		HEAL_LARGE = 100

		;

	const float
		ZOOM_SPEED = 20.0f,
		MOVE_SPEED = 5.0f,

		TRADER_RADIUS = 1.75f,

		ENEMYTYPE1_RANGE = 8.0f,
		ENEMYTYPE1_ATTACK_DURATION = 2.0f,
		ENEMYTYPE1_ATTACK_DELAY = float(8.0f), // definitely the attack duration - cant actually be less than other
		ENEMYTYPE1_MAXSPEED = 1.0f,
		ENEMYTYPE1_DRAG = 1.0f,

		ENEMYTYPE2_RANGE = 6.0f,
		ENEMYTYPE2_ATTACK_DURATION = 8.0f,
		ENEMYTYPE2_ATTACK_DELAY = float(4.0f), // definitely the attack duration - cant actually be less than other on normal timer
		ENEMYTYPE2_MAXSPEED = 3.0f,
		ENEMYTYPE2_DRAG = 3.0f,


		TILE_UV_INC = 1.0f/8.0f // tile is 1/8 of texture atlus
		;



	const float FOOTSTEP_PITCH[7]
	{
		-0.5f,
		-0.3f,
		-0.1f,
		0.0f,
		0.1f,
		0.3f,
		0.5f
	};
	const size_t FOOTSTEP_PITCH_COUNT = sizeof(GC::FOOTSTEP_PITCH) / sizeof(GC::FOOTSTEP_PITCH[0]);

	const float VOICE_PITCH[5]
	{
		-0.4f,
		-0.2f,
		0.0f,
		0.2f,
		0.4f
	};

	const size_t VOICE_PITCH_COUNT = sizeof(GC::VOICE_PITCH) / sizeof(GC::VOICE_PITCH[0]);

	const std::string
		GO_PLAYER = "Player",
		GO_ENEMY = "Enemy",
		GO_WEAPON = "Weapon",
		GO_TILE = "Tiles",
		GO_TRADER = "Trader",
		GO_POTATO = "Potato",

		// TODO: (NOTE) IMPLEMENT ENEMY TYPES HERE
		// ?? = NOT IMPLEMENTED OR NEEDS REVIEWING
		ENEMY_TYPE_1 = "EnemyType1", 
		// BASIC BARFING ENEMY (HOBBLES TOWARD PLAYER THEN BARFS, NO DAMAGE FROM CONTACT??)
		ENEMY_TYPE_2 = "EnemyType2", 
		// CHARGER ENEMY (FAST, NO PARTICLE ATTACK)


		STATE_PLAY = "PlayState",
		STATE_TRADE = "TradeState",
		STATE_PAUSE = "PauseMenu",
		STATE_MAINMENU = "MainMenu",
		STATE_GAMEOVER = "GameOver",
		STATE_WIN = "WinState",

		TRADER_NAME_TEST = "TravellerTest",
		TRADER_NAME_1 = "Traveller1",
		TRADER_NAME_2 = "Traveller2",
		TRADER_NAME_3 = "Traveller3",
		TRADER_NAME_4 = "Traveller4",

		TRADER_OBJ_1 = "Car boot storage",
		TRADER_OBJ_2 = "Car seats",
		TRADER_OBJ_3 = "Car engine",

		PLANT_NAME_0 = "Sour Potato",
		PLANT_NAME_1 = "Worm Herb",
		PLANT_NAME_2 = "Skull Rose"
		;

	const PlantData
		PLANT_0 = { PLANT_NAME_0 , 0.1f },
		PLANT_1 = { PLANT_NAME_1 , 0.05f },
		PLANT_2 = { PLANT_NAME_2 , 0.01f }
	;

	//audio
	const std::string TRADER_TALK[6]
	{
		"trader01",
		"trader02",
		"trader03",
		"trader04",
		"trader05",
		"trader06",
	};

	const size_t TRADER_TALK_COUNT = sizeof(TRADER_TALK) / sizeof(TRADER_TALK[0]);

	const DirectX::XMFLOAT2
		MENU_BUTTON_PIVOT{0.5f,0.6f},
		MENU_BUTTON_DISTANCE{200.0f,100.0f}, // distance from pivot
		MENU_TITLE_POSITION{20.0f,20.0f},
		MENU_BODY_POSITION{20.0f,50.0f},


		// Add this to uv coords for texture on atlus
		TILE_UV_STONE{ 0.0f , 0.0f },
		TILE_UV_JUNK{ 0.0f , 0.5f },
		TILE_UV_DANGER{ 0.125f , 0.0f },
		TILE_UV_GRASS{ 0.125f , 0.5f },
		TILE_UV_SPIKES{ 0.25f , 0.0f },
		TILE_UV_NOTHING{ 0.25f , 0.5f },
		TILE_UV_EMPTY{ 0.375f , 0.0f },
		TILE_UV_BLANK{ 0.375f , 0.5f }

	;
	const size_t NUM_TILE_UV_RAND = 4;

	// Add a random element to tile uvcoords for texture on atlus
	const DirectX::XMFLOAT2 TILE_UV_RANDOM[ NUM_TILE_UV_RAND ]{
		{	0.000f , 	0.000f },
		{	0.000f , 	0.125f },
		{	0.000f , 	0.250f },
		{	0.000f , 	0.375f } 
	};

	// eg stoneTileUV +=  GC::TILE_UV_STONE  + GC::TILE_UV_RANDOM[rand() % GC::NUM_TILE_UV_RAND]
	// eg dangerTileUV += GC::TILE_UV_DANGER + GC::TILE_UV_RANDOM[rand() % GC::NUM_TILE_UV_RAND]


	const DirectX::XMFLOAT3
		DIRECTION_FWD({ 0.0f,	0.0f,	1.0f }),
		DIRECTION_BCK({ 0.0f,	0.0f,	-1.0f }),
		DIRECTION_RGT({ 1.0f,	0.0f,	0.0f }),
		DIRECTION_LFT({ -1.0f,	0.0f,	0.0f }),
		DIRECTION_RAISE({ 0.0f,	1.0f,	0.0f }),
		DIRECTION_LOWER({ 0.0f,	-1.0f,	0.0f })
		;
	const DirectX::XMFLOAT4
		BUTTON_DOWN_COLOR({ 0.7f, 0.7f, 0.7f, 1.0f}),
		BUTTON_UP_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f },

		TEXT_DARK_COLOR{ 0.2f, 0.2f, 0.2f, 1.0f},
		TEXT_LIGHT_COLOR{ 1.0f, 1.0f, 1.0f, 1.0f }
		;

	
	const long
		// RECT
		PANEL_SRC[4]{ 192, 128,256,192 },

		BAR_MIN {32}, // for source rect limits 0% - 100%
		BAR_MAX { 160}, // for source rect limits 0% - 100%
		BAR_BG[4]	{ BAR_MIN,128,BAR_MAX,160 }, // Bar background
		BAR_GRN[4]	{ BAR_MIN,160,BAR_MAX,192 },
		BAR_YLW[4]	{ BAR_MIN,192,BAR_MAX,224 },
		BAR_RED[4]	{ BAR_MIN,224,BAR_MAX,256 },
		BAR_SIZE{ BAR_MAX - BAR_MIN } //max - min.
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
		{"Key To Valhalla",	{ItemCategory::KeyItems,  0}},
		{PLANT_NAME_0,		{ItemCategory::Farming, 100}},
		{PLANT_NAME_1,		{ItemCategory::Farming, 200}},
		{PLANT_NAME_2,		{ItemCategory::Farming, 350}},

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
		{ TRADER_NAME_TEST,			{{"Potion",1 }												}	},
		{ TRADER_NAME_1,			{{"Leadpipe",2 }											}	},
		{ TRADER_NAME_2,			{{"Potion",2}, {"Plastic Spork",2},{ "Holy Water",2}		}   },
		{ TRADER_NAME_3,			{{"Potion",2}, {"Nail Bat",2},{ "Leadpipe",2}				}   }, //todo change 
		{ TRADER_NAME_4,			{{"Potion",2}, {"Leadpipe",2},{ "Holy Water",2}				}   }, //todo change
		{ TRADER_OBJ_1,				{{"Potion",2}												}   }, // front of car  //todo change
		{ TRADER_OBJ_2,				{{"Potion",2}												}   }, // mid car       //todo change
		{ TRADER_OBJ_3,				{{"Potion",2}												}   }, // rear of car   //todo change
	};

	const ItemLookup ITEM_LOOKUP_REWARD
	{
		{ TRADER_NAME_TEST,		{{"Key To Valhalla",1 }											}	},
		{ TRADER_NAME_1,		{{"Leadpipe",2}													}	},
		{ TRADER_NAME_2,		{{"Potion",2}, {"Leadpipe",2},{ "Holy Water",2}					}	}, //
		{ TRADER_NAME_3,		{{"Potion",2}, {"Holy Water",2},{ "Holy Water",2}				}	}, //todo change 
		{ TRADER_NAME_4,		{																}	}, // no reward, charity
		{ TRADER_OBJ_1,			{																}   }, // front of car
		{ TRADER_OBJ_2,			{																}   }, // mid car
		{ TRADER_OBJ_3,			{																}   }, // rear of car
	};

	const ItemLookup PLANT_LOOKUP_HARVEST
	{
		{PLANT_NAME_0, {{ PLANT_NAME_0,1}}},
		{PLANT_NAME_1, {{ PLANT_NAME_1,1}}},
		{PLANT_NAME_2, {{ PLANT_NAME_2,1}}},
	};


	const std::map<std::string, DialogPair> DIALOG_PAIR
	{
		{ TRADER_NAME_TEST,	{"Got some rare things to trade, stranger","heh heh heh Thank you"	}},
		{ TRADER_NAME_1,	{"Trade with me","Now on your way"	}},
		{ TRADER_NAME_2,	{"I need some things   you need some things","Now I need some peace and quiet and you need to leave"	}}, //
		{ TRADER_NAME_3,	{"Trade with me","Now on your way"	}}, //todo change 
		{ TRADER_NAME_4, 	{"Trade with me","Now on your way"	}}, // no reward, charity
		{ TRADER_OBJ_3, 	{"There's lots of space to store weapons here.","Locked and loaded"	}},			// rear of car
		{ TRADER_OBJ_2, 	{"I have to load up on supplies for the journey","That should do it"}},			// middle of car
		{ TRADER_OBJ_1, 	{"I need to install these parts to get this moving","Purrs like a kitten"	}}	// front of car
	};
}
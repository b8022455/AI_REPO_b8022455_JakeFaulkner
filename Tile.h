#pragma once
#include <vector>
#include "GameObject.h"
#include <random>

struct mapData { // STRUCT FOR MAP TEXTURE AND TILE DATA
	int texIndex; //texture index from texture buffer for tile 
	//int x; // horizontal component - WIP FOR 3D
	//int y; // vertical component - WIP FOR 3D
};

class Tile : public GameObject
{

public:
	void Initialize(const std::string& renderItemName);
	void SetRandomPosition();
	void CreateTileWallHorizontal(int clumpSize, DirectX::XMFLOAT3& position, int count);
	void CreateTileWallVertical(int clumpSize, DirectX::XMFLOAT3& position, int count);
	DirectX::XMFLOAT3 SetRandom();
};


class GameTimer;
typedef std::vector<Tile> TileLine;
typedef std::vector<TileLine> TileGid; // 2D Tile Array

class TileManager 
{
	const int mDimention = 32;
	std::vector<std::vector<mapData>> mMapData;
	const float mUnitSize = 1.0f;
	TileGid  mTileGrid;


	const float mWidth = 30.0f;
	const float mHeight = 30.0f;
	const float mDepth = 30.0f;
	const float x = -0.5f*mWidth;
	const float z = -0.5f*mDepth;
	const float dx = mWidth / (mDimention - 1);
	const float dz = mDepth / (mDimention - 1);
	const std::string mRenderItemName = "Tiles";

public:
	void Initialize();
	void Update(const GameTimer& gt);
	Tile& GetTile(int x, int y);
	void SetTile(int x, int y, const Tile& tile);
	
};

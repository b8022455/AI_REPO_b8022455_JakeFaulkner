#include "Tile.h"

void TileManager::Initialize()
{

	//setup data array
	for (int k = 0; k < mDimention; ++k)
	{
		mMapData.push_back(std::vector<mapData>());

		for (int j = 0; j < mDimention; ++j)
		{
			mMapData.back().push_back(mapData());
			mMapData.back().back().texIndex = rand() % 3;
		}
	}


	// create tiles
	mTileGrid.reserve(mDimention);
	for (int k = 0; k < mDimention; ++k)
	{
		TileLine tl;
		tl.reserve(mDimention);
	
		for (int j = 0; j < mDimention; ++j)
		{
			tl.push_back(Tile());
			tl.back().Initialize(mRenderItemName); //Creates instances

			// Position instanced along a 3D grid.
			tl.back().mpInstance->World = DirectX::XMFLOAT4X4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				x + j * dx, 0.0f, z + k * dz, 1.0f);
	
			//Selects random index from array of textures WIP - will be using auto generated clumping 2d array
			tl.back().mpInstance->MaterialIndex = mMapData[j][k].texIndex; // set tile texture to struct data
		}
		
		mTileGrid.push_back(std::move(tl));
	
	}

}

void TileManager::Update(const GameTimer & gt)
{
	
}

Tile & TileManager::GetTile(int x, int y)
{
	return mTileGrid.at(y).at(x); //todo x,y may need swapping around
}

void TileManager::SetTile(int x, int y, const Tile & tile)
{
	mTileGrid.at(y).at(x) = tile; //todo x,y may need swapping around

}

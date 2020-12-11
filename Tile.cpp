#include "Tile.h"

void TileManager::Initialize()
{
	MaxGen = mDimention; // set up for tilefinder in playstate for player hazard detection

	// implement hazard tile creation in generation (can't be altered after?) 
	// TODO: implement player check for hazard tile
	// rescan tiles and sprinkle in hazards ? DOESN'T WORK
	//number of sprinkles, size of sprinkles (min,max), distance from other sprinkles of the same type

	int H1; // number of hazard spots to generate / health
	int H2; // number of hazard spots to generate / slow
	int H1MaxSize; // maximum size of hazard spot / health
	int H2MaxSize; // maximum size of hazard spot / slow
	int H1MinSize; // minimum size of hazard spot / health
	int H2MinSize; // minimum size of hazard spot / slow
	int H1Dist; // distance between other hazards / health
	int H2Dist; // distance between other hazards / slow

	//setup data array
	// possible error here with multiple texindexes conflicting?
	for (int k = 0; k < mDimention; ++k)
	{
		mMapData.push_back(std::vector<mapData>());

		for (int j = 0; j < mDimention; ++j)
		{
			mMapData.back().push_back(mapData());

			if (k == 0 || j == 0 || k == 31 || j == 31) { // DEBUG CODE, NEEDS REMOVING AFTER TESTING FINISHED
				mMapData.back().back().texIndex = 5;
			} // 4 = ICE, 5 = POISON/MARSH, 6 = MUD
			else {
				mMapData.back().back().texIndex = rand() % 2; // TODO: NOTE: TEXTURE IS RANDOMIZED HERE
			}
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
			// set all tiles to a default texture, this can then be used to determine the tile type
			//tl.back().mpInstance->MaterialIndex = 0; //0 = MOSS STONE BRICK, 1 = MOSS STONE, 2 LAB TILES,
			//3 = DEFUALT CRATE, 4 = ICE, 5 = POISON/MARSH, 6 = MUD, 7 = black now (was white), 8 = BLACK ONWARDS
		}
		
		mTileGrid.push_back(std::move(tl));
	
	}
}

void TileManager::Update(const GameTimer & gt)
{
	
}

int TileManager::GetIndex(int x, int y) {
	return mMapData[x][y].texIndex;
}

Tile & TileManager::GetTile(int x, int y)
{
	return mTileGrid.at(y).at(x); //todo x,y may need swapping around
}

void TileManager::SetTile(int x, int y, const Tile & tile)
{
	mTileGrid.at(y).at(x) = tile; //todo x,y may need swapping around

}
//----------------INDIVIDUAL TILE CODE-----------------------------------------------//

void Tile::Initialize(const std::string& renderItemName)
{
	AddRenderItemInstance("Tiles");
}

void Tile::InitTilePosition(int instance, DirectX::XMFLOAT3 position, int textIndex)
{
	mpInstance->MaterialIndex = textIndex;
	mpInstance->World._41 = position.x;
	mpInstance->World._42 = position.y;
	mpInstance->World._43 = position.z;
}

void Tile::SetPosition(const DirectX::XMFLOAT3& newPosition)
{
	//Updates position on the object
	mpInstance->World._41 = newPosition.x;
	mpInstance->World._42 = newPosition.y;
	mpInstance->World._43 = newPosition.z;
}

void Tile::SetRandomPosition()
{
	int min = -15;
	int max = 15;
	std::random_device rd;								// only used once to initialise (seed) engine
	std::mt19937 rng(rd());								// random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(min, max);	// guaranteed unbiased

	float random_integer = (float)uni(rng);
	float random_integer2 = (float)uni(rng);

	mpInstance->World._41 = random_integer;
	mpInstance->World._42 = 5.0f; // todo change back to 0.0f
	mpInstance->World._43 = random_integer2;
}

//DirectX::XMFLOAT3 Tile::GetPos() // inherited member not allowed?
//{
//	return {
//		mpInstance->World._41,
//		mpInstance->World._42,
//		mpInstance->World._43
//	};
//}
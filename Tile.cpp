#include "Tile.h"

void TileManager::Initialize()
{
	MaxGen = mDimention; // set up for tilefinder in playstate for player hazard detection

	srand(time(NULL));
	std::mt19937 rng(time(NULL)); // seed the generator

	std::uniform_int_distribution<int> grid(0,mDimention);
	std::uniform_int_distribution<int> gen1(H1MinSize, H1MaxSize); // uniform, unbiased
	std::uniform_int_distribution<int> gen2(H2MinSize, H2MaxSize); // uniform, unbiased
	std::uniform_int_distribution<int> gen3(H3MinSize, H3MaxSize); // uniform, unbiased

	std::vector<std::vector<int>> coords(mDimention);
	for (int u = 0; u < mDimention; u++) {
		coords[u].resize(mDimention);
	} // setup coords array to grid size
	
	std::vector<std::vector<int>> origin(mDimention); // used to store central tiles only during the first spawn
	for (int u = 0; u < mDimention; u++) {
		origin[u].resize(mDimention);
	} // setup coords array to grid size

	//int Max = H1 + H2 + H3; // total number of hazard sources

	// select positions for central hazard spots at random and store in vector
	// use distance here as well

	// hazard type 1 - find hazard spots and check not within distance of other hazard
	while (H1 > 0) { // until all hazards have been placed repeat (may cause issue )
		for (int i = 0; i < mDimention; i++) {
			for (int o = 0; o < mDimention; o++) {
				// calculate random variable to figure out hazard spot
				int r = rand() % dimSquare; // favors the lower numbers
				//int r = grid(rng); // causes a worse problem
				if (r <= 10 && H1 > 0) { // if tile is selected
					// DISTANCE CHECK HERE
					// main is central square (number to square)
					//			  o
					//	 o		 ooo
					//	ooo		ooooo
					//	 o		 ooo
					//			  o
					bool SAFE = true;
					int Cycles = (H1Dist * 2) + 1; // max number / for loop
					bool odd = true; // whether or not to run odd or even logic
					int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
					int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
					for (int d = 0; d < Cycles; d++) {
						bool set = false; 
						if (odd == true) { // larger loop
							for (int x = 0; x < (H1Dist + 1); x++) {
								// xgrid = i -H1Dist + Xcycle + x
								// ygrid = o + Ycycle - x
								int checkx = (i - H1Dist + Xcycle + x);
								int checky = (o + Ycycle - x);
								if ((i - H1Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - H1Dist + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention)
									if (coords[i - H1Dist + Xcycle + x][o + Ycycle - x] == 5)
										SAFE = false;
							}
							Xcycle++;
						} // X ISNT CALCULATED PROPERLY, H1DIST + 1 ISN'T WORKING
						if (odd == false) { // smaller loop
							for (int x = 0; x < H1Dist; x++) {
								// xgrid = i(-H1Dist+1) + Xcycle + x
								// ygrid = o + Ycyle - x
								int checkx = (i - H1Dist + Xcycle + x);
								int checky = (o + Ycycle - x);
								if ((i - H1Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - H1Dist + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention)
									if (coords[i - H1Dist + Xcycle + x][o + Ycycle - x] == 5)
										SAFE = false;
							}
							Ycycle++;
						}
						if (odd == true && set == false) {
							odd = false;
							set = true;
						}
						if (odd == false && set == false) {
							odd = true;
							set = true;
						}
					}

					if (SAFE == true) {
						coords[i][o] = 5;
						origin[i][o] = 5; // 
						H1 -= 1;
					}
				}
			}
		}
		//H1 -= 1;
	}

	// hazard type 2 - find hazard spots and check not within distance of other hazard
	while (H2 > 0) { // until all hazards have been placed repeat (may cause issue )
		for (int i = 0; i < mDimention; i++) {
			for (int o = 0; o < mDimention; o++) {
				// calculate random variable to figure out hazard spot
				int r = rand() % dimSquare;
				//int r = grid(rng); // causes worse problems
				if (r <= 10 && H2 > 0) { // if tile is selected
					// DISTANCE CHECK HERE
					// main is central square (number to square)
					//			  o
					//	 o		 ooo
					//	ooo		ooooo
					//	 o		 ooo
					//			  o
					bool SAFE = true;
					int Cycles = (H2Dist * 2) + 1; // max number / for loop
					bool odd = true; // whether or not to run odd or even logic
					int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
					int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
					for (int d = 0; d < Cycles; d++) {
						bool set = false;
						if (odd == true) { // larger loop
							for (int x = 0; x < (H2Dist + 1); x++) {
								// xgrid = i -H1Dist + Xcycle + x
								// ygrid = o + Ycycle - x
								if ((i - H2Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - H2Dist + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention)
									if (coords[i - H2Dist + Xcycle + x][o + Ycycle - x] == 6)
										SAFE = false;
							}
							Xcycle++;
						}
						if (odd == false) { // smaller loop
							for (int x = 0; x < H2Dist; x++) {
								// xgrid = i(-H1Dist+1) + Xcycle + x
								// ygrid = o + Ycyle - x
								if ((i - H2Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - H2Dist + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention)
									if (coords[i - H2Dist + Xcycle + x][o + Ycycle - x] == 6)
										SAFE = false;
							}
							Ycycle++;
						}
						if (odd == true && set == false) {
							odd = false;
							set = true;
						}
						if (odd == false && set == false) {
							odd = true;
							set = true;
						}
					}

					if (SAFE == true) {
						coords[i][o] = 6;
						origin[i][o] = 6;
						H2 -= 1;
					}
				}
			}
		}
	}

	// hazard type 3 - find hazard spots and check not within distance of other hazard
	while (H3 > 0) { // until all hazards have been placed repeat (may cause issue )
		for (int i = 0; i < mDimention; i++) {
			for (int o = 0; o < mDimention; o++) {
				// calculate random variable to figure out hazard spot
				int r = rand() % dimSquare;
				//int r = grid(rng); // causes a worse problem
				if (r <= 10 && H3 > 0) { // if tile is selected
					// DISTANCE CHECK HERE
					// main is central square (number to square)
					//			  o
					//	 o		 ooo
					//	ooo		ooooo
					//	 o		 ooo
					//			  o
					bool SAFE = true;
					int Cycles = (H3Dist * 2) + 1; // max number / for loop
					bool odd = true; // whether or not to run odd or even logic
					int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
					int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
					for (int d = 0; d < Cycles; d++) {
						bool set = false;
						if (odd == true) { // larger loop
							for (int x = 0; x < (H3Dist + 1); x++) {
								// xgrid = i -H1Dist + Xcycle + x
								// ygrid = o + Ycycle - x
								if ((i - H3Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - H3Dist + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention)
									if (coords[i - H3Dist + Xcycle + x][o + Ycycle - x] == 4)
										SAFE = false;
							}
							Xcycle++;
						}
						if (odd == false) { // smaller loop
							for (int x = 0; x < H3Dist; x++) {
								// xgrid = i(-H1Dist+1) + Xcycle + x
								// ygrid = o + Ycyle - x
								if ((i - H3Dist + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
									(i - H3Dist + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention)
									if (coords[i - H3Dist + Xcycle + x][o + Ycycle - x] == 4)
										SAFE = false;
							}
							Ycycle++;
						}
						if (odd == true && set == false) {
							odd = false;
							set = true;
						}
						if (odd == false && set == false) {
							odd = true;
							set = true;
						}
					}

					if (SAFE == true) {
						coords[i][o] = 4;
						origin[i][o] = 4;
						H3 -= 1;
					}
				}
			}
		}
		//H1 -= 1;
	}

	// set range of hazard using min & max size and set the nearby tiles as such

	// COULD TRY TO IMPLEMENT RANDOMNESS HERE AS WELL, SAVE SPACE AND 
	
	for (int i = 0; i < mDimention; i++) {
		for (int o = 0; o < mDimention; o++) {
			// use origin to select central tiles & set all tiles within gen 
			if (origin[i][o] == 5){ // if central tile found
				int haz1 = gen1(rng); // random between min & max
				// use for loop to set all tiles within gen1 to hazard
				int Cycles = (int(haz1) * 2) + 1; // max number / for loop
				bool odd = true; // whether or not to run odd or even logic
				int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (haz1 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - haz1 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz1 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								coords[i - haz1 + Xcycle + x][o + Ycycle - x] = 5; // sets coord to hazard
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < haz1; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							int checkx = (i - haz1 + Xcycle + x);
							int checky = (o + Ycycle - x);
							if ((i - haz1 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz1 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								coords[i - haz1 + Xcycle + x][o + Ycycle - x] = 5;
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
				// RANDOMNESS HERE
				// use H1RandomDist as max distance from central AND randomness is the range within dimsquare to use?
				
				int Rand1 = H1RandomDist + H1Dist; // set
				// use for loop to set all tiles within gen1 to hazard
				Cycles = (Rand1 * 2) + 1; 
				odd = true; // whether or not to run odd or even logic
				Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (Rand1 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - Rand1 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand1 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								if (coords[i - Rand1 + Xcycle + x][o + Ycycle - x] != 5) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % dimSquare);
									if (use <= H1Random)
										coords[i - Rand1 + Xcycle + x][o + Ycycle - x] = 5;
								}
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < Rand1; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							//int checkx = (i - haz1 + Xcycle + x); // DEBUG
							//int checky = (o + Ycycle - x); // DEBUG
							if ((i - Rand1 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand1 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								if (coords[i - Rand1 + Xcycle + x][o + Ycycle - x] != 5) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % dimSquare);
									if (use <= H1Random)
										coords[i - Rand1 + Xcycle + x][o + Ycycle - x] = 5;
								}
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
			}
			// HAZARD 2
			if (origin[i][o] == 6) { // if central tile found
				int haz2 = gen2(rng); // random between min & max
				// use for loop to set all tiles within gen1 to hazard
				int Cycles = (int(haz2) * 2) + 1; // max number / for loop
				bool odd = true; // whether or not to run odd or even logic
				int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (haz2 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - haz2 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz2 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								coords[i - haz2 + Xcycle + x][o + Ycycle - x] = 6; // sets coord to hazard
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < haz2; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							if ((i - haz2 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz2 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								coords[i - haz2 + Xcycle + x][o + Ycycle - x] = 6;
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
				// RANDOMNESS HERE
				// use H1RandomDist as max distance from central AND randomness is the range within dimsquare to use?

				int Rand2 = H2RandomDist + H2Dist; // set
				// use for loop to set all tiles within gen1 to hazard
				Cycles = (Rand2 * 2) + 1;
				odd = true; // whether or not to run odd or even logic
				Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (Rand2 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - Rand2 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand2 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								if (coords[i - Rand2 + Xcycle + x][o + Ycycle - x] != 6) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % dimSquare);
									if (use <= H2Random)
										coords[i - Rand2 + Xcycle + x][o + Ycycle - x] = 6;
								}
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < Rand2; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							//int checkx = (i - haz1 + Xcycle + x); // DEBUG
							//int checky = (o + Ycycle - x); // DEBUG
							if ((i - Rand2 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand2 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								if (coords[i - Rand2 + Xcycle + x][o + Ycycle - x] != 6) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % dimSquare);
									if (use <= H2Random)
										coords[i - Rand2 + Xcycle + x][o + Ycycle - x] = 6;
								}
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
			}
			// HAZARD 3
			if (origin[i][o] == 4) { // if central tile found
				int haz3 = gen3(rng); // random between min & max
				// use for loop to set all tiles within gen1 to hazard
				int Cycles = (int(haz3) * 2) + 1; // max number / for loop
				bool odd = true; // whether or not to run odd or even logic
				int Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				int Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (haz3 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - haz3 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz3 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								coords[i - haz3 + Xcycle + x][o + Ycycle - x] = 4; // sets coord to hazard
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < haz3; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							if ((i - haz3 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - haz3 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								coords[i - haz3 + Xcycle + x][o + Ycycle - x] = 4;
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
				// RANDOMNESS HERE
				// use H1RandomDist as max distance from central AND randomness is the range within dimsquare to use?

				int Rand3 = H3RandomDist + H3Dist; // set
				// use for loop to set all tiles within gen1 to hazard
				Cycles = (Rand3 * 2) + 1;
				odd = true; // whether or not to run odd or even logic
				Xcycle = 0; // used for x increase at start (increased after each whole odd cycle) 
				Ycycle = 0; // used for y increase at start (increased after each whole even cycle)
				for (int d = 0; d < Cycles; d++) {
					bool set = false;
					if (odd == true) { // larger loop
						for (int x = 0; x < (Rand3 + 1); x++) {
							// xgrid = i -H1Dist + Xcycle + x
							// ygrid = o + Ycycle - x
							if ((i - Rand3 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand3 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								if (coords[i - Rand3 + Xcycle + x][o + Ycycle - x] != 4) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % dimSquare);
									if (use <= H3Random)
										coords[i - Rand3 + Xcycle + x][o + Ycycle - x] = 4;
								}
						}
						Xcycle++;
					}
					if (odd == false) { // smaller loop
						for (int x = 0; x < Rand3; x++) {
							// xgrid = i(-H1Dist+1) + Xcycle + x
							// ygrid = o + Ycyle - x
							//int checkx = (i - haz1 + Xcycle + x); // DEBUG
							//int checky = (o + Ycycle - x); // DEBUG
							if ((i - Rand3 + Xcycle + x) >= 0 && (o + Ycycle - x) >= 0 &&
								(i - Rand3 + Xcycle + x) < mDimention && (o + Ycycle - x) < mDimention) // error catcher
								if (coords[i - Rand3 + Xcycle + x][o + Ycycle - x] != 4) { // not already a hazard
									// ADJUST HERE AS NEEDED
									int use = (rand() % dimSquare);
									if (use <= H3Random)
										coords[i - Rand3 + Xcycle + x][o + Ycycle - x] = 4;
								}
						}
						Ycycle++;
					}
					if (odd == true && set == false) {
						odd = false;
						set = true;
					}
					if (odd == false && set == false) {
						odd = true;
						set = true;
					}
				}
			}
		}
	}

	// use randomness attributed to that hazard to activate / deactivate other tiles outside up to range of 2 & 
	// interior tiles, use gen value + 2


	// 4 = ICE, 5 = POISON/MARSH, 6 = MUD

	//setup data array
	for (int k = 0; k < mDimention; ++k)
	{
		mMapData.push_back(std::vector<mapData>());

		for (int j = 0; j < mDimention; ++j)
		{
			mMapData.back().push_back(mapData());

			if (coords[k][j] <= 0)
				mMapData.back().back().texIndex = rand() % 2; // random plain tile
			else
				mMapData.back().back().texIndex = coords[k][j]; // set as hazard

			//if (k == 0 || j == 0 || k == 31 || j == 31) { // DEBUG CODE, NEEDS REMOVING AFTER TESTING FINISHED
			//	mMapData.back().back().texIndex = 5;
			//} // 4 = ICE, 5 = POISON/MARSH, 6 = MUD
			//else {
			//	mMapData.back().back().texIndex = rand() % 2; // TODO: NOTE: TEXTURE IS RANDOMIZED HERE
			//}
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
	mpInstance->World._42 = 4.0f; // height above ground level
	mpInstance->World._43 = random_integer2;

}

void Tile::CreateTileWallHorizontal(int clumpSize, DirectX::XMFLOAT3& position, int count)
{
	mpInstance->World._41 = position.x + count;
	mpInstance->World._42 = 2.0f; // height above ground level
	mpInstance->World._43 = position.z;
}

void Tile::CreateTileWallVertical(int clumpSize, DirectX::XMFLOAT3& position, int count)
{
	mpInstance->World._41 = position.x;
	mpInstance->World._42 = 2.0f; // height above ground level
	mpInstance->World._43 = position.z + count;
}


DirectX::XMFLOAT3 Tile::SetRandom()
{
	int min = -15;
	int max = 15;
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(min, max);

	float random_integer = (float)uni(rng);
	float random_integer2 = (float)uni(rng);


	return DirectX::XMFLOAT3
	{
		random_integer,
		1,
		random_integer2
	};
}
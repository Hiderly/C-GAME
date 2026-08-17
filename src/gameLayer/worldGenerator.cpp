#include "worldGenerator.h"
#include "randomStuff.h"

#include<FastNoiseSIMD.h>


void generateWorld(GameMap &gameMap, int seed)
{
	const int w = 900;
	const int h = 500;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed++);

	std::unique_ptr<FastNoiseSIMD> dirtNoiseGenrator(FastNoiseSIMD::NewFastNoiseSIMD());
	//std::unique_ptr<FastNoiseSIMD> stoneNoiseGenrator(FastNoiseSIMD::NewFastNoiseSIMD());

	dirtNoiseGenrator->SetSeed(seed++);
//	stoneNoiseGenrator->SetSeed(seed++);

	dirtNoiseGenrator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	dirtNoiseGenrator->SetFractalOctaves(1);  // 1层噪声
	dirtNoiseGenrator->SetFrequency(0.02f);   // 变化频率

/*	stoneNoiseGenrator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	stoneNoiseGenrator->SetFractalOctaves(4);   
	stoneNoiseGenrator->SetFrequency(0.01f);   */

	// 生成噪声
	float* dirtNoise = FastNoiseSIMD::GetEmptySet(w); // 返回指向数组大小为w的指针  
	//float* stoneNoise = FastNoiseSIMD::GetEmptySet(w);
	dirtNoiseGenrator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);
	//stoneNoiseGenrator->FillNoiseSet(stoneNoise, 0, 0, 0, w, 1, 1);


	
	// 将生成的噪声值从 [-1 1] 映射到[0 1]
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoise[i] + 1) / 2;
		//stoneNoise[i] = (stoneNoise[i] + 1) / 2;
	}

	int dirtOffsetStart = -5;
	int dirtOffsetEnd = 35;

	//int stoneHeightStart = 80;
	//int stoneHeightEnd = 170;

	int keepDirectionTimeStone = getRandomInt(rng, 5, 40);
	int directionStone = getRandomInt(rng, -2, 2);
	
	int stoneHeight = 90;

	for (int x = 0; x < w; x++)
	{
		keepDirectionTimeStone--;
		if (keepDirectionTimeStone <= 0)
		{
			keepDirectionTimeStone = getRandomInt(rng, 5, 40);
			directionStone = getRandomInt(rng, -2, 2);
		}

		if (directionStone == -2)
		{
			if (getRandomChance(rng, 0.25f))
			{
				stoneHeight--;
			}
			if (getRandomChance(rng, 0.25f))
			{
				stoneHeight--;
			}
		}

		else if (directionStone == -1)
		{
			if (getRandomChance(rng, 0.25f))
			{
				stoneHeight--;
			}
		}

		else if (directionStone == 1)
		{
			if (getRandomChance(rng, 0.25f))
			{
				stoneHeight++;
			}
		}

		else if (directionStone == 2)
		{
			if (getRandomChance(rng, 0.25f))
			{
				stoneHeight++;
			}
			if (getRandomChance(rng, 0.25f))
			{
				stoneHeight++;
			}
		}
		
		if (stoneHeight <= 60)
		{
			stoneHeight = 60;
		}

		if (stoneHeight >= 120)
		{
			stoneHeight = 120;
		}

		int dirtHeight = dirtOffsetStart + (dirtOffsetEnd - dirtOffsetStart) * dirtNoise[x];   // [-5  35]
		dirtHeight = stoneHeight - dirtHeight;  // 生成泥土的区间：[25  125]  石头的区间：[60, 120]

		for (int y = 0; y < h; y++)
		{
			Block b = {};
	
			if (y > dirtHeight)
			{
				b.type = Block::dirt;
			}
			if (y == dirtHeight)
			{
				b.type = Block::grassBlock;
			}
			if (y > stoneHeight)
			{
				b.type = Block::stone;
				if (getRandomChance(rng, 0.08f)) { b.type = Block::gold; }
			}

			gameMap.getBlockUnsafe(x, y) = b;
		}//for(y)
	}//for(x)

	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
}



/*	for (int x = 0; x < w; x++)
	{
		int stoneHeight = stoneHeightEnd + (stoneHeightEnd - stoneHeightStart) * stoneNoise[x];  // [170  260]
		int dirtHeight = dirtOffsetStart + (dirtOffsetEnd - dirtOffsetStart) * dirtNoise[x];   // [-5  35]
		dirtHeight = stoneHeight - dirtHeight;  // [135  265]
		for (int y = 0; y < h; y++)
		{
			Block b;
			if (y > dirtHeight)
			{
				b.type = Block::dirt;
			}
			if (y == dirtHeight)
			{
				b.type = Block::grassBlock;
			}
			if (y > stoneHeight)
			{
				b.type = Block::stone;
			}

			gameMap.getBlockUnsafe(x, y) = b;
		}
	}*/


//	FastNoiseSIMD::FreeNoiseSet(stoneNoise);


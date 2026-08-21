#include "worldGenerator.h"
#include "randomStuff.h"
#include"struct.h"
#include"saveMap.h"

#include<FastNoiseSIMD.h>


void generateWorld(GameMap &gameMap, int seed)
{
	const int w = 900;
	const int h = 500;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed++);

	//加载树结构
	Structure treeStructure;
	loadBlockDataFromFile(treeStructure.mapData, treeStructure.w, treeStructure.h, RESOURCES_PATH "structures/tree.bin");

	// 沙漠
	int desertStart = getRandomInt(rng, 10, w - 210); // [10  690]
	int desertWeight = getRandomInt(rng, 50, 100);
	int desertEnd = desertStart + desertWeight;
	if (desertEnd >= w) { desertEnd = w; }

	std::unique_ptr<FastNoiseSIMD> dirtNoiseGenrator(FastNoiseSIMD::NewFastNoiseSIMD());
	std::unique_ptr<FastNoiseSIMD> cavesNoiseGenrator(FastNoiseSIMD::NewFastNoiseSIMD());

	dirtNoiseGenrator->SetSeed(seed++);
	cavesNoiseGenrator->SetSeed(seed++);

	dirtNoiseGenrator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	dirtNoiseGenrator->SetFractalOctaves(6);   // 1层噪声
	dirtNoiseGenrator->SetFractalGain(0.4f);  // 每增加一层 octave，新一层的影响强度降低多少
	dirtNoiseGenrator->SetFrequency(0.02f);  // 变化频率

	cavesNoiseGenrator->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
	cavesNoiseGenrator->SetFractalOctaves(3);
	cavesNoiseGenrator->SetFrequency(0.02f);

	// 生成噪声
	float* dirtNoise = FastNoiseSIMD::GetEmptySet(w); // 返回指向数组大小为w的指针  
	dirtNoiseGenrator->FillNoiseSet(dirtNoise, 0, 0, 0, w, 1, 1);

	float* cavesNoise = FastNoiseSIMD::GetEmptySet(w * h);
	cavesNoiseGenrator->FillNoiseSet(cavesNoise, 0, 0, 0, h, w, 1);
	
	// 将生成的噪声值从 [-1 1] 映射到[0 1]
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoise[i] + 1) / 2;
	}

	for (int i = 0; i < w * h; i++)
	{
		cavesNoise[i] = (cavesNoise[i] + 1) / 2;
	}

	auto getCavesNoise = [&](int x, int y) 
		{
			return cavesNoise[x + y * w];
		};



	int dirtOffsetStart = -5;
	int dirtOffsetEnd = 35;

	int keepDirectionTimeStone = getRandomInt(rng, 5, 40);  // 生成山脉走向的时间
	int directionStone = getRandomInt(rng, -2, 2);
	
	int stoneHeight = 90;
	int dessertHeight = 90; // 沙漠高度

	int dirtType = Block::dirt;
	int grassType = Block::grassBlock;
	int stoneType = Block::stone;

	for (int x = 0; x < w; x++)
	{
		bool isInDesert = (x >= desertStart && x <= desertEnd);
		if (isInDesert)
		{
			dirtType = Block::sand;
			grassType = Block::sand;
			stoneType = Block::sand;
		}
		if (!isInDesert)
		{
			 dirtType = Block::dirt;
			 grassType = Block::grassBlock;
			 stoneType = Block::stone;
		}

		keepDirectionTimeStone--;
		if (keepDirectionTimeStone <= 0)
		{
			keepDirectionTimeStone = getRandomInt(rng, 5, 40);   // 生成山脉走向的时间
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

		else if (directionStone == 2)  // 下坡
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
				b.type = dirtType;
			}
			if (y == dirtHeight)
			{
				b.type = grassType;
			}
			if (y > stoneHeight)
			{
				b.type = stoneType;
				if (getRandomChance(rng, 0.08f) && !isInDesert) { b.type = Block::gold; }
			}

			if (isInDesert)
			{
				int desertMid_X = (desertEnd + desertStart) / 2;
				int desertMid_W = (desertEnd - desertStart) / 2;
				int distanceToMid_X = std::abs(x - desertMid_X);
				float desertDistance = 1 - distanceToMid_X / (float)desertMid_W;

				int desertDepth_min = dessertHeight + 10;
				int desertDepth_max = dessertHeight + 20;
				int desertDepth = desertDepth_min + desertDistance * desertDepth_max;
				
				if (y > desertDepth)
				{
					b.type = Block::stone;
					if (getRandomChance(rng, 0.08f)) { b.type = Block::gold; }
				}
			}

			if (getCavesNoise(x, y) < 0.3f)
			{
				b.type = Block::air;
			}

			gameMap.getBlockUnsafe(x, y) = b;
		}//for(y)
	}//for(x)

	FastNoiseSIMD::FreeNoiseSet(dirtNoise);
	FastNoiseSIMD::FreeNoiseSet(cavesNoise);

	// 柏林蠕虫
	for (int i = 0; i < 20; i++)
	{
		float x = getRandomInt(rng, 10, w - 10);
		float y = getRandomInt(rng, 150, h - 10);

		float dir_x = getRandomFloat(rng, -1, 1);
		float dir_y = getRandomFloat(rng, -1, 1);

		int worm_len = getRandomInt(rng, 150, 600);
		float radius = 2.5f;

		int changeDirection_time = getRandomInt(rng, 5, 20);

		for (int j = 0; j < worm_len; j++)
		{
			int intRadius = std::ceil(radius);
			// 消除周围圆形方块
			for (int ox = -intRadius; ox <= intRadius; ox++)
			{
				for (int oy = -intRadius; oy <= intRadius; oy++)
				{
					int distSq = ox * ox + oy * oy;
					if (distSq <= intRadius * intRadius)
					{
						int digX = x + ox;
						int digY = y + oy;
						auto b = gameMap.getBlockSafe(digX, digY);
						if (b)
						{
							b->type = Block::air;
						}
					}
				}
			}//

			// 时间到改变蠕虫方向
			changeDirection_time--;
			if (changeDirection_time <= 0)
			{
				changeDirection_time = getRandomInt(rng, 5, 20);
				if (getRandomChance(rng, 0.7))
				{
					float keepFactor = 0.8;
					dir_x = dir_x * keepFactor + getRandomInt(rng, -1, 1) * (1 - keepFactor);
					dir_y = dir_y * keepFactor + getRandomInt(rng, -1, 1) * (1 - keepFactor);
				}
				else
				{
					float keepFactor = 0.2;
					dir_x = dir_x * keepFactor + getRandomInt(rng, -1, 1) * (1 - keepFactor);
					dir_y = dir_y * keepFactor + getRandomInt(rng, -1, 1) * (1 - keepFactor);
				}
			}
			//

			// 蠕虫向前移动
			x += dir_x * 1.5;
			y += dir_y * 1.5;
			// 改变蠕虫半径
			radius += getRandomFloat(rng, -0.2, 0.2);
			radius = std::clamp(radius, 2.2f, 6.5f);
		}
	}

#pragma region Creat_trees
	for (int x = 0; x < w; x++)
	{
		if (getRandomChance(rng, 0.2))
		{
			for (int y = 0; y < h; y++)
			{
				auto type = gameMap.getBlockUnsafe(x, y).type;
				if (type == Block::air) continue;
				if (type == Block::grassBlock)
				{
					Vector2 spawnPos{ (float)x, (float)y };
					spawnPos.x -= treeStructure.w / 2;
					spawnPos.y = y - treeStructure.h;
					treeStructure.pasteIntoMap(gameMap, spawnPos);

					x += (treeStructure.w / 2 + 2);
					break;
				}
				else
				{
					break;
				}
			}
		}
	}
}


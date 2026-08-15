#include<raylib.h>
#include "gameMain.h"
#include<asserts.h>
#include<iostream>
#include "assetManager.h"
#include"gameMap.h"

struct GameData
{
	GameMap gameMap;
}gameData;

AssetManager assetManager; // 游戏数据和资源是不同的东西

bool initGame()
{
	assetManager.loadAll();

	gameData.gameMap.create(30, 10);
	gameData.gameMap.getBlockUnsafe(0, 0).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(1, 1).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(2, 2).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(3, 3).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(4, 4).type = Block::dirt;


	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime(); // 上一帧到当前帧之间经过了多少秒
	if (deltaTime > 1.f / 200) { deltaTime = 1.f / 200; }

	ClearBackground({75, 75, 150, 225});

	for (int y = 0; y < gameData.gameMap.h; y++)
	{
		for (int x = 0; x < gameData.gameMap.w; x++)
		{
			auto& b = gameData.gameMap.getBlockUnsafe(x, y);
			if (b.type == Block::dirt)
			{
				float size = 32;
				float posX = x * 32;
				float posY = y * 32;

				DrawTexturePro(assetManager.dirt,
					{ 0, 0, (float)assetManager.dirt.width, (float)assetManager.dirt.height },
					{ posX, posY, size, size },
					{},
					0,
					WHITE);
			}
		}
	}



	return true;
}

void closeGame()
{

}
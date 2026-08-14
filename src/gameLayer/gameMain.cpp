#include<raylib.h>
#include "gameMain.h"
#include<asserts.h>
#include<iostream>
#include "assetManager.h"

struct GameData
{

}gameData;

AssetManager assetManager; // 游戏数据和资源是不同的东西

bool initGame()
{
	assetManager.loadAll();
	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime(); // 上一帧到当前帧之间经过了多少秒
	if (deltaTime > 1.f / 200) { deltaTime = 1.f / 200; }

	DrawTexturePro(assetManager.dirt, 
		{ 0, 0, (float)assetManager.dirt.width, (float)assetManager.dirt.height },
		{50, 50, 100, 100}, 
		{}, 
		0, 
		WHITE);

	return true;
}

void closeGame()
{

}
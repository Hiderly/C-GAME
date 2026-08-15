#include<raylib.h>
#include<raymath.h>
#include<asserts.h>
#include<iostream>

#include "gameMain.h"
#include "assetManager.h"
#include"gameMap.h"
#include"helpers.h"

struct GameData
{
	GameMap gameMap = {};
	Camera2D camera = {};
}gameData;

AssetManager assetManager; // 游戏数据和资源是不同的东西

bool initGame()
{
	assetManager.loadAll(); // 加载所有游戏资源

	gameData.gameMap.create(30, 10);
	gameData.gameMap.getBlockUnsafe(0, 0).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(1, 1).type = Block::grassBlock;
	gameData.gameMap.getBlockUnsafe(2, 2).type = Block::sand;
	gameData.gameMap.getBlockUnsafe(3, 3).type = Block::sandRuby;
	gameData.gameMap.getBlockUnsafe(4, 4).type = Block::sandStone;
//	gameData.gameMap.getBlockUnsafe(0, -1).type = Block::stone;

	gameData.camera.target = { 0, 0 };
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 100.0f;
	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime(); // 上一帧到当前帧之间经过了多少秒
	if (deltaTime > 1.f / 200) { deltaTime = 1.f / 200; }
	ClearBackground({ 75, 75, 150, 225 });

	BeginMode2D(gameData.camera); // 启用相机，之后渲染改为世界坐标而不是屏幕坐标

#pragma region Map
	Vector2 topLeftView = GetScreenToWorld2D({ 0, 0 }, gameData.camera);
	Vector2 bottomRightView = GetScreenToWorld2D(
		{(float)GetScreenWidth(), (float)GetScreenHeight()}, 
		gameData.camera);

	int startXView = (int)floorf(topLeftView.x - 1);
	int endXView = (int)ceilf(bottomRightView.x + 1);
	int startYView = (int)floorf(topLeftView.y - 1);
	int endYView = (int)ceilf(bottomRightView.y + 1);
	
	startXView = Clamp(startXView, 0, gameData.gameMap.w - 1);
	endXView = Clamp(endXView, 0, gameData.gameMap.w - 1);
	startYView = Clamp(startYView, 0, gameData.gameMap.h - 1);
	endYView = Clamp(endYView, 0, gameData.gameMap.h - 1);

	for (int Y = startYView; Y <= endYView; Y++)
	{
		for (int X = startXView; X <= endXView; X++)
		{
			auto& b = gameData.gameMap.getBlockUnsafe(X, Y);
			if (b.type != Block::air)
			{
				float size = 1;
				Rectangle textureUV;
				textureUV.x = b.type * 32;
				textureUV.y = 0;
				textureUV.width = 32;
				textureUV.height = 32;

				DrawTexturePro(
					assetManager.texture,
					getTextureAtlas(b.type, 0, 32, 32),
					{ (float)X, (float)Y, size, size },
					{ 0, 0 },
					0.0f,
					WHITE);
			}
		}
	}
#pragma endregion

#pragma region Camare_Move
	if (IsKeyDown(KEY_LEFT))  { gameData.camera.target.x -= 7.0f * deltaTime; }
	if (IsKeyDown(KEY_RIGHT)) { gameData.camera.target.x += 7.0f * deltaTime; }
	if (IsKeyDown(KEY_UP))    { gameData.camera.target.y -= 7.0f * deltaTime; }
	if (IsKeyDown(KEY_DOWN))  { gameData.camera.target.y += 7.0f * deltaTime; }
#pragma endregion

#pragma region Get_Mouse_Pos
	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera); // 将鼠标从屏幕坐标转为世界坐标
	int blockX = floorf(worldPos.x);
	int blockY = floorf(worldPos.y);
#pragma endregion

#pragma region Mouse_Pos_Block
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
		if (b)
		{
			*b = {};
		}
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
		if (b)
		{
			b->type = Block::gold;
		}
	}
#pragma endregion


/*	for (int y = 0; y < gameData.gameMap.h; y++)
	{
		for (int x = 0; x < gameData.gameMap.w; x++)
		{
			auto& b = gameData.gameMap.getBlockUnsafe(x, y);
			if (b.type != Block::air)
			{
				float size = 1;
				Rectangle textureUV;
				textureUV.x = b.type * 32;
				textureUV.y = 0;
				textureUV.width = 32;
				textureUV.height = 32;

				DrawTexturePro(
					assetManager.texture,
					getTextureAtlas(b.type, 0, 32, 32),
					{ (float)x, (float)y, size, size },
					{0, 0},
					0.0f,
					WHITE);
			}
		}
	} */

#pragma region Mouse_Pos_Frame
	DrawTexturePro(
		assetManager.frame,
		{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height },
		{ (float)blockX, (float)blockY, 1, 1 }, 
		{ },
		0.0f,
		WHITE);
#pragma endregion

	EndMode2D();
	DrawFPS(10, 10);
	return true;
}

void closeGame()
{

}
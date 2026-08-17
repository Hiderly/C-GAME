#include<raylib.h>
#include<raymath.h>
#include<asserts.h>
#include<iostream>
#include<random>
#include<imgui.h>

#include "gameMain.h"
#include "assetManager.h"
#include"gameMap.h"
#include"helpers.h"
#include"randomStuff.h"
#include"worldGenerator.h"


struct GameData
{
	GameMap gameMap = {};
	Camera2D camera = {};
}gameData;

AssetManager assetManager; // 游戏资源管理器


bool initGame()
{
	// 加载所有游戏资源: 纹理
	assetManager.loadAll(); 

	//生成世界
	generateWorld(gameData.gameMap);

	//摄像机初始设置
	gameData.camera.target = { 20, 68 };
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

#pragma region Init_Map
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
	static float CAMERA_SPEED = 10.0f;
	if (IsKeyDown(KEY_LEFT))  { gameData.camera.target.x -= CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_RIGHT)) { gameData.camera.target.x += CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_UP))    { gameData.camera.target.y -= CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_DOWN))  { gameData.camera.target.y += CAMERA_SPEED * deltaTime; }
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

#pragma region ImGUI
	ImGui::Begin("Game Controll");
	ImGui::SliderFloat("Camera Zoom:", &gameData.camera.zoom, 3, 100);
	ImGui::SliderFloat("Camera Speed:", &CAMERA_SPEED, 5, 100);
	ImGui::End();
#pragma endregion

	DrawFPS(10, 10);
	return true;
}

void closeGame()
{

}
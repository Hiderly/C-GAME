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
#include"struct.h"
#include"saveMap.h"
#include"physics.h"

struct GameData
{
	GameMap gameMap = {};
	Camera2D camera = {};
	
	int creativeSelectedBlock = Block::dirt;

	Vector2 selectionStart = {};
	Vector2 selectionEnd = {};

	Structure copyStructure;
	char saveName[100] = {};

	PhysicalEntity player;
}gameData;

AssetManager assetManager; // 游戏资源管理器

bool showImGUI = false;

bool initGame()
{
	// 加载所有游戏资源: 纹理
	assetManager.loadAll(); 
	// 生成世界
	generateWorld(gameData.gameMap);
	// 摄像机初始设置
	gameData.camera.target = { 20, 68 };
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 50.0f;
	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
	// 玩家
	gameData.player.teleport({ 20.5, 68.5 });
	gameData.player.transform.w = 0.8f;
	gameData.player.transform.h = 1.8f;

	return true;
}

bool updateGame()
{
	if (IsKeyPressed(KEY_Q)) { showImGUI = !showImGUI; }
	
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
	static float CAMERA_SPEED = 100.0f;
	if (IsKeyDown(KEY_LEFT))  { gameData.player.transform.pos.x -= CAMERA_SPEED * GetFrameTime(); }
	if (IsKeyDown(KEY_RIGHT)) { gameData.player.transform.pos.x += CAMERA_SPEED * GetFrameTime(); }
	if (IsKeyDown(KEY_UP))    { gameData.player.transform.pos.y -= CAMERA_SPEED * GetFrameTime(); }
	if (IsKeyDown(KEY_DOWN))  { gameData.player.transform.pos.y += CAMERA_SPEED * GetFrameTime(); }
#pragma endregion

#pragma region Player
	gameData.player.addGravity();
	gameData.player.updateForces(deltaTime);
	gameData.camera.target = gameData.player.transform.pos;
	gameData.player.updateFinal();
#pragma endregion

#pragma region Get_Mouse_Pos
	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera); // 将鼠标从屏幕坐标转为世界坐标
	int blockX = floorf(worldPos.x);
	int blockY = floorf(worldPos.y);
#pragma endregion

	if (gameData.creativeSelectedBlock < 0) { gameData.creativeSelectedBlock = 0; }
	if (gameData.creativeSelectedBlock >= Block::BLOCKS_COUNT) { gameData.creativeSelectedBlock = Block::BLOCKS_COUNT - 1; }


#pragma region SelectionBlocks
	if (showImGUI)
	{
		if (IsKeyPressed(KEY_ONE)) { gameData.selectionStart = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyPressed(KEY_TWO)) { gameData.selectionEnd = Vector2{ (float)blockX, (float)blockY }; }
		if (IsKeyPressed(KEY_THREE))
		{
			gameData.copyStructure.pasteIntoMap(gameData.gameMap, Vector2{ (float)blockX, (float)blockY });
		}
		if (gameData.selectionStart.x > gameData.selectionEnd.x)
		{
			std::swap(gameData.selectionStart.x, gameData.selectionEnd.x);
		}
		if (gameData.selectionStart.y > gameData.selectionEnd.y)
		{
			std::swap(gameData.selectionStart.y, gameData.selectionEnd.y);
		}
	}
#pragma endregion


#pragma region Mouse_Pos_Block
	if (!showImGUI)
	{
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
				b->type = gameData.creativeSelectedBlock;
			}
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


#pragma region SelectionBlocks_ImGUI
	if (showImGUI)
	{
		Rectangle rec;
		rec.x = gameData.selectionStart.x;
		rec.y = gameData.selectionStart.y;
		rec.width = gameData.selectionEnd.x - gameData.selectionStart.x;
		rec.height = gameData.selectionEnd.y - gameData.selectionStart.y;
		rec.width++;
		rec.height++;
		DrawRectangleLinesEx(rec, 0.1, { 20, 101, 250, 145 });
	}
#pragma endregion

#pragma region Draw_player
	Transform2D playerSprite = gameData.player.transform;
	playerSprite.w = 1;
	playerSprite.h = 2;
	playerSprite.pos.y -= (playerSprite.h - gameData.player.transform.h) / 2;  // (20.5 68.4)
	DrawTexturePro(
		assetManager.player,
		{ 0, 0, (float)assetManager.player.width, (float)assetManager.player.height },
		playerSprite.getAABB(), // (20 67.4)
		{ 0, 0 }, // top-left 位置
		0.0f,
		WHITE
	);
	DrawRectangleLinesEx(gameData.player.transform.getAABB(), 0.1, { 20, 101, 250, 120 });
#pragma endregion

#pragma region Collision_Test
	Transform2D test;
	test.pos = { 20.5, 68.5 };
	test.w = 1;
	test.h = 1;

	Transform2D mouse;
	mouse.pos = worldPos;
	mouse.w = 1;
	mouse.h = 1;
	if (test.intersectTrasndform(mouse))
	{
		DrawRectangleLinesEx(test.getAABB(), 0.1, RED);
		DrawRectangleLinesEx(mouse.getAABB(), 0.1, RED);
	}
	else {
		DrawRectangleLinesEx(test.getAABB(), 0.1, GREEN);
		DrawRectangleLinesEx(mouse.getAABB(), 0.1, GREEN);
	}
#pragma endregion

	EndMode2D(); // 关闭相机, 弃用世界坐标

#pragma region ImGUI
	if (showImGUI)
	{
		ImGui::Begin("Game Controll");
		ImGui::SliderFloat("Camera Zoom:", &gameData.camera.zoom, 3, 100);
		ImGui::SliderFloat("Camera Speed:", &CAMERA_SPEED, 5, 150);

		if (ImGui::Button("Copy"))
		{
			gameData.copyStructure.copyFromMap(gameData.gameMap, gameData.selectionStart, gameData.selectionEnd);
		}

		ImGui::InputText("File_name", gameData.saveName, sizeof(gameData.saveName)); 

		if (ImGui::Button("Save to file"))
		{
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			saveBlockDataToFile(gameData.copyStructure.mapData,
				gameData.copyStructure.w,
				gameData.copyStructure.h,
				path.c_str());
		}
		if (ImGui::Button("Load from file"))
		{
			std::string path = RESOURCES_PATH "structures/";
			path += gameData.saveName;
			path += ".bin";

			loadBlockDataFromFile(gameData.copyStructure.mapData,
				gameData.copyStructure.w,
				gameData.copyStructure.h,
				path.c_str());
		}

		ImGui::Separator();

		for (int i = 0; i < Block::BLOCKS_COUNT; i++)
		{
			auto atlas = getTextureAtlas(i, 0, 32, 32);
			atlas.x /= assetManager.texture.width;
			atlas.width /= assetManager.texture.width;
			atlas.y /= assetManager.texture.height;
			atlas.height /= assetManager.texture.height;

			ImGui::PushID(i);
			ImTextureID tex = (ImTextureID)(intptr_t)assetManager.texture.id;
			if (ImGui::ImageButton(
				tex, 
				{ 35, 35 }, 
				{ atlas.x, atlas.y }, 
				{ atlas.x + atlas.width, atlas.y + atlas.height }))
			{
				gameData.creativeSelectedBlock = i;
			}
			ImGui::PopID();
			if (i % 10 != 0)
			{
				ImGui::SameLine();
			}
		}
		ImGui::End();
	}
#pragma endregion


	DrawFPS(10, 10);
	return true;
}

void closeGame()
{

}
#include<raylib.h>
#include "gameMain.h"

struct GameData
{
	float rec_start_pox = 100;
	float rec_start_poy = 100;
}gameData;

bool initGame()
{
	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime(); // 上一帧到当前帧之间经过了多少秒
	if (deltaTime > 1.f / 200) { deltaTime = 1.f / 200; }
	if (IsKeyDown(KEY_A)) { gameData.rec_start_pox -= 200 * deltaTime; }
	if (IsKeyDown(KEY_D)) { gameData.rec_start_pox += 200 * deltaTime; }
	if (IsKeyDown(KEY_W)) { gameData.rec_start_poy -= 200 * deltaTime; }
	if (IsKeyDown(KEY_S)) { gameData.rec_start_poy += 200 * deltaTime; }
	DrawRectangle(gameData.rec_start_pox, gameData.rec_start_poy, 20, 20, { 255, 0, 0, 255 });
	return true;
}

void closeGame()
{

}
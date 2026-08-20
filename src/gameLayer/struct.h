#pragma once
#include<vector>
#include<raylib.h>
#include<gameMap.h>

#include"blocks.h"
struct Structure
{
	int w = 0;
	int h = 0;
	std::vector<Block> mapData;

	void create(int w, int h);
	Block& getBlockUnsafe(int x, int y);
	Block* getBlockSafe(int x, int y);  

	void copyFromMap(GameMap& gamemap, Vector2 start, Vector2 end);
	void pasteIntoMap(GameMap& gamemap, Vector2 start);
};  
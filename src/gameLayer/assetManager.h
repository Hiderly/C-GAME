#pragma once
#include<raylib.h>

struct AssetManager
{
	Texture2D dirt = {};
	Texture2D texture = {};
	Texture2D frame = {}; // 显示鼠标对应位置方块

	void loadAll();
};
#include"struct.h"

#include<asserts.h>
#include<algorithm>

void Structure::create(int w, int h)
{
	*this = {};
	mapData.resize(w * h);
	this->w = w;
	this->h = h;

	for (auto& e : mapData) { e = {}; }
}

Block& Structure::getBlockUnsafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() == w * h, "map data not initialized");
	permaAssertCommentDevelopement(x >= 0 && y >= 0 && x < w && y < h, "getBlocUnsafe out of bounds error ");

	return mapData[x + y * w];
}

Block* Structure::getBlockSafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() == w * h, "map data not initialized");

	if (x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }
	return &mapData[x + y * w];
}


void Structure::copyFromMap(GameMap& gamemap, Vector2 start, Vector2 end)
{
	if (start.x >= gamemap.w || end.x >= gamemap.w || start.y >= gamemap.h || end.y >= gamemap.h)
	{
		start.x = 0; start.y = 0; end.x = 0; end.y = 0;
	}
	if (start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
	{
		start.x = 0; start.y = 0; end.x = 0; end.y = 0;
	}

	if (start.x > end.x) { std::swap(start.x, end.x); }
	if (start.y > end.y) { std::swap(start.y, end.y); }

	int width = end.x - start.x + 1;
	int height = end.y - start.y + 1;
	if (width > gamemap.w) { return; }
	if (height > gamemap.h) { return; }
//	Vector2 size = Vector2{ (end.x - start.x + 1), (end.y - start.y + 1) };
//	if (size.x > gamemap.w) { return; }
//	if (size.y > gamemap.h) { return; }
	
	create(width, height);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			getBlockUnsafe(x, y) = gamemap.getBlockUnsafe(x + start.x, y + start.y);
		}
	}
}


void Structure::pasteIntoMap(GameMap& gamemap, Vector2 start)
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			auto b = gamemap.getBlockSafe(x + start.x, y + start.y);
			if (b)
			{
				*b = getBlockUnsafe(x, y);
			}
		}
	}
}





#include"saveMap.h"
#include<asserts.h>

bool saveBlockDataToFile(std::vector<Block> blocks, int w, int h, const char* file_name)
{
	std::ofstream f(file_name, std::ios::binary);
	
	if (!f.is_open()) { return false; }

	permaAssertDevelopement(blocks.size() == w * h);
	permaAssertDevelopement(blocks.size() != 0);
	if (blocks.size() != w * h) return false;
	if (blocks.size() == 0)return false;

	f.write((const char*)&w, sizeof(w));  // f.write(地址,字节数量)
	f.write((const char*)&h, sizeof(h));
	f.write((const char*)blocks.data(), sizeof(Block) * blocks.size());

	f.close();
	return true;
}


bool loadBlockDataFromFile(std::vector<Block>& blocks, int& w, int& h, const char* file_name)
{
	blocks.clear();
	w = 0;
	h = 0;

	std::ifstream f(file_name, std::ios::binary);
	if (!f.is_open()) return false;

	f.read((char*)&w, sizeof(w));  // f.read(地址,字节数量)
	f.read((char*)&h, sizeof(h));

	if (!f || w <= 0 || h <= 0)
	{
		f.close();
		return false;
	}

	if (w > 10000) { f.close(); return false; }
	if (h > 600) { f.close(); return false; }

	size_t blockCount = w * h;
	blocks.resize(blockCount);

	f.read((char*)blocks.data(), sizeof(Block) * blockCount);

	if (!f)
	{
		blocks.clear();
		w = 0;
		h = 0;
		f.close();
		return false;
	}

	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i].sanitize();
	}

	f.close();
	return true;
}
#include"saveMap.h"
#include<asserts.h>

struct BlockSaveRepresentation1
{
	std::uint16_t type = 0;
	Block toBlock()
	{
		Block b;
		b.type = type;
		return b;
	}
};

struct BlockSaveRepresentation2
{
	std::uint16_t type = 0;
	std::uint8_t life = 0;
	Block toBlock()
	{
		Block b;
		b.type = type;
		b.life = life;
		return b;
	}
};

const int VERSION = 2;

BlockSaveRepresentation2 toBlockRepresentation(Block b)
{
	BlockSaveRepresentation2 rez;
	rez.type = b.type;
	rez.life = b.life;
	return rez;
}

bool saveBlockDataToFile(const std::vector<Block>& blocks, int w, int h, const char* filename)
{
	std::ofstream f(filename, std::ios::binary);
	
	if (!f.is_open()) { return false; }

	permaAssertDevelopement(blocks.size() == w * h);
	permaAssertDevelopement(blocks.size() != 0);
	if (blocks.size() != w * h) return false;
	if (blocks.size() == 0)return false;

	f.write((const char*)&VERSION, sizeof(VERSION));
	f.write((const char*)&w, sizeof(w));  // f.write(地址,字节数量)
	f.write((const char*)&h, sizeof(h));

	for (int i = 0; i < blocks.size(); i++)
	{
		auto b = toBlockRepresentation(blocks[i]);
		f.write((const char*)&b, sizeof(b));
	}

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

	int readVersion = 0;
	f.read((char*)&readVersion, sizeof(readVersion));
	f.read((char*)&w, sizeof(w));  // f.read(地址,字节数量)
	f.read((char*)&h, sizeof(h));

	if (!f || w <= 0 || h <= 0)
	{
		f.close();
		return false;
	}

	if (w > 10000) { f.close(); return false; }
	if (h > 600) { f.close(); return false; }

	switch (readVersion)
	{

	case 1:
	{
		size_t blockCount = w * h;
		blocks.resize(blockCount);
		for (int i = 0; i < blockCount; i++)
		{
			BlockSaveRepresentation1 read;
			f.read((char*)&read, sizeof(read));
			if (!f)
			{
				blocks.clear();
				w = 0;
				h = 0;
				f.close();
				return false;
			}
			blocks[i] = read.toBlock();
		}
		break;
	}

	case 2:
	{
		size_t blockCount = w * h;
		blocks.resize(blockCount);
		for (int i = 0; i < blockCount; i++)
		{
			BlockSaveRepresentation2 read;
			f.read((char*)&read, sizeof(read));
			if (!f)
			{
				blocks.clear();
				w = 0;
				h = 0;
				f.close();
				return false;
			}
			blocks[i] = read.toBlock();
		}
		break;
	}

	default:
	{
		w = 0;
		h = 0;
		f.close();
		return false;
	}

	}

	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i].sanitize();
	}

	f.close();
	return true;
}
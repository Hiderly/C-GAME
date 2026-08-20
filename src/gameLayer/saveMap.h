#pragma once
#include<vector>
#include<blocks.h>
#include<fstream>

bool saveBlockDataToFile(const std::vector<Block>& blocks, int w, int h, const char* file_name);

bool loadBlockDataFromFile(std::vector<Block>& blocks, int& w, int& h, const char* file_name);
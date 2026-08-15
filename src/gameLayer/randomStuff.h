#pragma once
#include<random>

//返回float类型 [min, max)   
float getRandomFloat(std::ranlux24_base& rng, float min, float max); //std::ranlux24_base& rng 创建随机数引擎rng

//返回int类型 [min, max]
int getRandomInt(std::ranlux24_base& rng, int min, int max);


bool getRandomChance(std::ranlux24_base& rng, float chance);
// ShortTestsForCp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <random>
#include <vector>

void Test2(int r,int n, int m)
{
	uint32_t s = 32,s1=1;
	while ( s1 != m)
	{
		s1 <<= 1;
		--s;
	}
	
	std::mt19937 gen(r);
	std::uniform_int_distribution<uint32_t> dist1(0, 0xffff);
	std::uniform_int_distribution<uint32_t> dist2(0, 0xffffffff);

	std::vector<uint32_t> a1;
	std::vector<uint32_t> a2;

	// duplikaty
	for (int i = 0; i < n; ++i)
	{
		auto ii = dist1(gen);
		a1.push_back(ii);
	}


	for (int i = 0; i < m; ++i)
		a2.push_back(0);

	uint32_t i1, i2,c;
	for (c = 1;; ++c)
	{
		i1 = dist2(gen);
		i2 = dist2(gen);
		for (int i = 0; i < m; ++i)
			a2[i] = 0;
		bool ok = true;
		for (int i = 0; i < n; ++i)
		{
			auto j = (a1[i] * i1 + i2)>>s;
			if (a2[j] != 0)
			{
				ok = false;
				break;
			}
			a2[j] = 1;
		}

		if (ok)
			break;
	}

	std::cout << c << " " << i1 << " " << i2 << std::endl;
	std::cout << "ok";
}

void Test1(uint16_t j)
{
	uint32_t a[16] = {};
	uint32_t i = 0;
	do
	{
		uint16_t i1 = i & 0xFFFF;
		uint16_t i2 = i >> 16;
		uint16_t i3 = uint16_t(j * i1 + i2) >> 12;
		++a[i3];
		if (i3 >= 16)
			std::cout << "xx";
		if (i % (1024 * 1024) == 0)
			std::cout << double(i) / 40000000 <<std::endl;

	} while (++i!=0);

	for (int i = 0; i < 16; ++i)
		if (a[i] != 1 << 28)
		{
			std::cout << "xxxx";
			return;
		}
	std::cout << "ok";
}

int main()
{
	void TestFindHash(int rand_seed, int max_number, int test_count, int hash_count, bool randquick_nums, bool randquick_hash);
	void TestFindDoubleHash(int rand_seed, int max_number, int test_count, int hash_count, int max_rehash, bool randquick_nums, bool randquick_hash);
	void TestStatHash(int rand_seed, int max_number, int test_count, int hash_count, int stat_count, bool randquick_nums, bool randquick_hash);

	if (0)
	for (int i = 0; i < 111; ++i)
		TestFindDoubleHash(7755+i, 16*1024, 4*256, 4*256, 1000, false, false);
	
	if (0)
	for (int i=0;i<1101;++i)
		TestFindHash(7755+i,1024, 128,512, true, true);
	
	TestStatHash(7765, 8*1024, 1024, 2048, 1000, true, true);
	return 0;

	
}

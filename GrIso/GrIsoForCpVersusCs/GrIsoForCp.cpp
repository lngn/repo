// ShortTestsForCp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <random>
#include <vector>


int main()
{
	void TestGraphIso(unsigned rand_seed, int vertex_count, int edge_count);
	//TestGraphIso(34787, 1000, 700 * 1000 / 2); std::cin.get(); return 0;

	void TestGraphIso(unsigned rand_seed_graph, unsigned rand_seed_perm, int vertex_count, int edge_count, bool save);
	TestGraphIso(313, 313, 5000, 200 * 5000 / 2, true); std::cin.get(); return 0;

	void OkTest(); return 0;

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

#include "GraphIso.h"
#include <chrono>
#include <iostream>

namespace GrIso
{
	double ElapsedTime()
	{
		using double_seconds = std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::ratio<1,1>>>;
		static double_seconds last = std::chrono::steady_clock::now();
		double_seconds curr = std::chrono::steady_clock::now();
		double seconds = (curr-last).count();
		last = curr;
		return seconds;
	}

	class OkTests
	{
	public:
		RandQuick rand_quick;
        
		void Assert(bool b)
		{
			if (!b)
			{
				printf("Fail");
				throw std::exception("Assert");
			}
		}

		OkTests(uint rand_seed=1313)
		{
			rand_quick = RandQuick(rand_seed);
		}

		bool TestOneShortListCorrectness(uint list_size, uint item_size, uint seed_test = 0, uint seed_hash = 0)
		{
			if (seed_test != 0)
				rand_quick.last = seed_test;
			else
				seed_test = rand_quick.last;
			if (seed_hash != 0)
				RandQuickShared.last = seed_hash;
			else
				seed_hash = RandQuickShared.last;
			printf("%d %d %d %d\n",list_size, item_size, seed_test, seed_hash);

			std::set<short> hash_test;
			while (hash_test.size() < list_size)
				hash_test.insert((ushort)rand_quick.Next(0, item_size));
			ShortHashList hash_list;
			for (auto i : hash_test)
				hash_list.Append(i);

			bool b = hash_list.Hash();
			if (!b)
			{
				printf("Hash not found %d\n",list_size);
				return false;
			}

			for (auto i : hash_test)
				Assert(hash_list.Find(i));
			for (ushort i = 0; i <= item_size; ++i)
				Assert((hash_test.count(i)>0) == hash_list.Find(i));

			return true;
		}

		void TestAllShortListCorrectness(int max_list_size, int test_count=20)
		{
			int hash_fails = 0, hash_count = 0;
			for (;test_count > 0; --test_count)
			{
				printf("Test no %d\n", test_count);
				for (int list_size = 1; list_size < max_list_size; ++list_size)
					for (int item_size = list_size - 1; item_size < 1024; ++item_size)
					{
						++hash_count;
						if (!TestOneShortListCorrectness(list_size, item_size))
							++hash_fails;
					}
			}
			printf("Hash fails %d of %d\n", hash_fails, hash_count);
		}

		void TestGraphCompare(int vertex_count, int edge_count)
		{
			Graph graph = Graph::Generate(vertex_count, edge_count);
			//Graph clone = graph;
			//Assert(graph.Compare(clone));
			Assert(!graph.Compare(Graph::Generate(vertex_count, edge_count)));
			std::vector<int> permutation = Graph::Permutate(vertex_count);
			Graph perm = graph.Permutate( permutation);
			Assert(!graph.Compare(perm));
			Assert(graph.Compare(perm,permutation));
		}

		void TestGraphCompare()
		{
			for (int c=25;c>0;--c)
			for (int vertex_count = 8; vertex_count < 24; ++vertex_count)
				for (int edge_count = vertex_count * (vertex_count - 1) / 8; edge_count < vertex_count * (vertex_count - 1) / 4; ++edge_count)
					TestGraphCompare(vertex_count, edge_count);
		}

		void PrintRandGraphPermutation(int vertex_count, int edge_count)
		{
			Graph graph = Graph::Generate(vertex_count, edge_count);
			std::vector<int> permutation = Graph::Permutate(vertex_count);
			Graph perm = graph.Permutate(permutation);
			Assert(!graph.Compare(perm));
			Assert(graph.Compare( perm, permutation));
			printf(graph.ToString().c_str()); printf("\n");
			printf(perm.ToString().c_str()); printf("\n");
		}

		void TestGraphIso(int vertex_count, int edge_count)
		{
			//Debug.WriteLine($"rand seed {RandQuick.Shared.last}");
			Graph graph = Graph::Generate(vertex_count, edge_count);
			std::vector<int> permutation = Graph::Permutate(vertex_count);
			Graph perm = graph.Permutate(permutation);
			//Debug.WriteLine(graph.ToString());
			//Debug.WriteLine(perm.ToString());
			const std::vector<int> & result = GraphIso(graph, perm);
				Assert(graph.Compare(perm, result));
		}

		int TestGraphNoIso(int vertex_count, int edge_count)
		{
			//Debug.WriteLine($"rand seed {RandQuick.Shared.last}");
			Graph graph = Graph::Generate(vertex_count, edge_count);
			//Debug.WriteLine(graph.ToString());
			//Debug.WriteLine(perm.ToString());
			const std::vector<int> & result = GraphIso(graph, Graph::Generate(vertex_count, edge_count));
			return result.empty() ? 1 : 0;
		}

		void TestGraphIso()
		{
			for (int vertex_count = 3; vertex_count < 10; ++vertex_count)
				for (int edge_count = vertex_count; edge_count < vertex_count * (vertex_count - 1) / 2; ++edge_count)
					for (int count = 0; count < 50; ++count)
					{
						if (count==0)
							printf("%d %d\n", vertex_count, edge_count);
						TestGraphIso(vertex_count, edge_count);
					}

			for (int vertex_count = 10; vertex_count < 16; ++vertex_count)
				for (int edge_count = vertex_count * (vertex_count - 1) / 4; edge_count < vertex_count * (vertex_count - 1) * 3 / 4; ++edge_count)
					for (int count = 0; count < 50; ++count)
					{
						if (count == 0)
							printf("%d %d\n", vertex_count, edge_count);
						TestGraphIso(vertex_count, edge_count);
					}

			int noiso = 0, total = 0;
			for (int vertex_count = 10; vertex_count < 16; ++vertex_count)
				for (int edge_count = vertex_count*(vertex_count-1)/4; edge_count < vertex_count * (vertex_count - 1)*3 /4; ++edge_count)
					for (int count = 0; count < 50; ++count)
					{
						if (count == 0)
							printf("%d %d\n", vertex_count, edge_count);
						++total;
						noiso += TestGraphNoIso(vertex_count, edge_count);
					}
			Assert(noiso < total/100);
		}

		void TestGraphIso(uint rand_seed, int vertex_count, int edge_count)
		{

		}
	};
}

void OkTest()
{
	GrIso::ElapsedTime();

	GrIso::OkTests tests;
	tests.TestGraphIso();
	tests.TestGraphCompare();
	tests.TestAllShortListCorrectness(39);
}
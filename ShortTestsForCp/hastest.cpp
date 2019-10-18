#include "pch.h"
#include <iostream>
#include <random>
#include <vector>
#include "BaseKit.h"

void TestFindDoubleHash(int rand_seed, int max_number, int test_count, int hash_count, int max_rehash, bool randquick_nums, bool randquick_hash)
{
	std::mt19937 gen(rand_seed);
	RandQuick rand_quick(rand_seed);

	uint32_t hash_shift = 32, shift_check = 1;
	while (shift_check != 0 && shift_check != hash_count)
	{
		shift_check <<= 1;
		--hash_shift;
	}
	if (shift_check == 0)
	{
		std::cout << "invalid shift" << std::endl;
	}

	std::vector<uint32_t> test_numbers;
	std::uniform_int_distribution<uint32_t> dist_test_numbers(0, max_number);
	for (int c = 0; c < test_count; )
	{
		uint32_t n = randquick_nums ? rand_quick(0, max_number) : dist_test_numbers(gen);
		for (int i = 0; ; ++i)
		{
			if (i == c)
			{
				test_numbers.push_back(n);
				++c;
				break;
			}
			else if (n == test_numbers[i])
				break;
		}
	}

	std::uniform_int_distribution<uint32_t> dist_test_hash(0, 0xffffffff);
	std::vector<uint32_t> test_hash;
	for (int i = 0; i < hash_count; ++i)
		test_hash.push_back(0);


	uint32_t hash_mul, hash_add, hash_check, hash_marker=0, rehash_first;
	for (hash_check = 1;; ++hash_check)
	{
		hash_mul = randquick_hash ? rand_quick.Next() : dist_test_hash(gen);
		hash_add = randquick_hash ? rand_quick.Next() : dist_test_hash(gen);

		++hash_marker;
		rehash_first = test_count;
		for (int i = 0; i < rehash_first; ++i)
		{
			uint32_t j = (test_numbers[i] * hash_mul + hash_add) >> hash_shift;
			if (test_hash[j] == hash_marker)
			{
				--rehash_first;
				uint32_t n = test_numbers[rehash_first];
				test_numbers[rehash_first] = test_numbers[i];
				test_numbers[i] = n;
			}
			else
			{
				test_hash[j] = hash_marker;
			}
		}

		bool ok = true;
		if (rehash_first != test_count)
		{
			for (uint32_t rehash_check = hash_check + max_rehash;hash_check<rehash_check;++hash_check)
			{
				hash_mul = randquick_hash ? rand_quick.Next() : dist_test_hash(gen);
				hash_add = randquick_hash ? rand_quick.Next() : dist_test_hash(gen);

				++hash_marker;
				ok = true;
				for (int i = rehash_first; i < test_count; ++i)
				{
					uint32_t j = (test_numbers[i] * hash_mul + hash_add) >> hash_shift;
					if (test_hash[j] == hash_marker)
					{
						ok = false;
						break;
					}
					else
					{
						test_hash[j] = hash_marker;
					}
				}
				if (ok)
					break;
			}
		}
		
		if (ok)
			break;
	}

	std::cout << hash_check << " " << hash_mul << " " << hash_add << std::endl;
}

void TestFindHash(int rand_seed, int max_number, int test_count, int hash_count, bool randquick_nums, bool randquick_hash)
{
	std::mt19937 gen(rand_seed);
	RandQuick rand_quick(rand_seed);

	uint32_t hash_shift = 32, shift_check = 1;
	while (shift_check !=0 &&  shift_check != hash_count)
	{
		shift_check <<= 1;
		--hash_shift;
	}
	if (shift_check == 0)
	{
		std::cout << "invalid shift" << std::endl;
	}

	std::vector<uint32_t> test_numbers;
	std::uniform_int_distribution<uint32_t> dist_test_numbers(0, max_number);
	for (int c = 0; c < test_count; )
	{
		uint32_t n = randquick_nums ? rand_quick(0,max_number) : dist_test_numbers(gen);
		for (int i = 0; ; ++i)
		{
			if (i == c)
			{
				test_numbers.push_back(n);
				++c;
				break;
			}
			else if (n == test_numbers[i])
				break;
		}
	}

	std::uniform_int_distribution<uint32_t> dist_test_hash(0, 0xffffffff);
	std::vector<uint32_t> test_hash;
	for (int i = 0; i < hash_count; ++i)
		test_hash.push_back(0);


	uint32_t hash_mul, hash_add, hash_check;
	for (hash_check = 1;; ++hash_check)
	{
		hash_mul = randquick_hash ? rand_quick.Next() : dist_test_hash(gen);
		hash_add = randquick_hash ? rand_quick.Next() : dist_test_hash(gen);
		
		bool ok = true;
		for (int i = 0; i < test_count; ++i)
		{
			uint32_t j = (test_numbers[i] * hash_mul + hash_add) >> hash_shift;
			if (test_hash[j] == hash_check)
			{
				ok = false;
				break;
			}
			test_hash[j] = hash_check;
		}

		if (ok)
			break;
	}

	std::cout << hash_check << " " << hash_mul << " " << hash_add << std::endl;	
}

void TestHashConflict(uint8_t j)
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
			std::cout << double(i) / 40000000 << std::endl;

	} while (++i != 0);

	for (int i = 0; i < 16; ++i)
		if (a[i] != 1 << 28)
		{
			std::cout << "xxxx";
			return;
		}
	std::cout << "ok";
}
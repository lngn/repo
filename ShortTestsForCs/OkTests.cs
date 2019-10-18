using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace GrIso
{
    class OkTests
    {
        RandQuick rand_quick;

        void Assert(bool b)
        {
            if (!b)
            {
                Console.WriteLine("Fail");
            }
        }

        public OkTests(uint rand_seed=1313)
        {
            rand_quick = new RandQuick(rand_seed);
        }

        public bool TestOneShortListCorrectness(int list_size, int item_size, uint seed_test = 0, uint seed_hash = 0)
        {
            if (seed_test != 0)
                rand_quick.last = seed_test;
            else
                seed_test = rand_quick.last;
            if (seed_hash != 0)
                RandQuick.Shared.last = seed_hash;
            else
                seed_hash = RandQuick.Shared.last;
            Console.WriteLine($"{list_size} {item_size} {seed_test} {seed_hash}");

            var hash_test = new HashSet<ushort>();
            while (hash_test.Count < list_size)
                hash_test.Add((ushort)rand_quick.Next(0, item_size));
            var hash_list = new ShortHashList();
            foreach (var i in hash_test)
                hash_list.Append(i);

            bool b = hash_list.Hash();
            if (!b)
            {
                Console.WriteLine("Hash not found {list_size}");
                return false;
            }

            foreach (var i in hash_test)
                Assert(hash_list.Find(i));
            for (ushort i = 0; i <= item_size; ++i)
                Assert(hash_test.Contains(i) == hash_list.Find(i));

            return true;
        }

        public void TestAllShortListCorrectness(int max_list_size, int test_count=20)
        {
            int hash_fails = 0, hash_count = 0;
            for (;test_count > 0; --test_count)
            {
                Console.WriteLine($"Test no {21 - test_count}");
                for (int list_size = 1; list_size < max_list_size; ++list_size)
                    for (int item_size = list_size - 1; item_size < 1024; ++item_size)
                    {
                        ++hash_count;
                        if (!TestOneShortListCorrectness(list_size, item_size))
                            ++hash_fails;
                    }
            }
            Console.WriteLine($"Hash fails {hash_fails} of {hash_count}");
        }

        public void TestShortListPerformace(int max_list_size)
        {
            int fake = 0;
            Console.WriteLine("Find(list size;ref time ms;hash time ms;)");
            for (int list_size = 1; list_size < max_list_size; ++list_size)
            {
                var hash_test = new HashSet<ushort>();
                while (hash_test.Count < list_size)
                    hash_test.Add((ushort)rand_quick.Next(0, 0xfff));
                var hash_list = new ShortHashList();
                foreach (var i in hash_test)
                    hash_list.Append(i);

                bool b = hash_list.Hash();
                Assert(b);

                var watch = Stopwatch.StartNew();
                
                for (int c = 1000000; c > 0; --c)
                    fake += hash_test.Contains((ushort)c) ? 1 : 0;
                long t1 = watch.ElapsedMilliseconds;
                for (int c = 1000000; c > 0; --c)
                    fake += hash_list.Find((ushort)c) ? 1 : 0;
                long t2 = watch.ElapsedMilliseconds;

                Console.WriteLine($"{list_size} {t1} {t2 - t1}");
            }

            Console.WriteLine("Enum(list size;ref time ms;hash time ms;)");
            for (int list_size = 1; list_size < max_list_size; ++list_size)
            {
                var hash_test = new HashSet<ushort>();
                while (hash_test.Count < list_size)
                    hash_test.Add((ushort)rand_quick.Next(0, 0xfff));
                var hash_list = new ShortHashList();
                foreach (var i in hash_test)
                    hash_list.Append(i);

                bool b = hash_list.Hash();
                Assert(b);

                var watch = Stopwatch.StartNew();

                for (int c = 100000; c > 0; --c)
                    foreach (var i in hash_test)
                        fake += i;
                long t1 = watch.ElapsedMilliseconds;

                for (int c = 100000; c > 0; --c)
                    for (var i = hash_list.First(); i != ShortHashList.None; i = hash_list.Next(i)) 
                        fake += hash_list.Find((ushort)c) ? 1 : 0;
                long t2 = watch.ElapsedMilliseconds;

                Console.WriteLine($"{list_size} {t1} {t2 - t1}");
            }
            Console.WriteLine($"fake {fake}");
        }
    }
        
}
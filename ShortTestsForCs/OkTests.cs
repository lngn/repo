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
                throw new AbortException("Assert");
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

        public bool TestOneVeryShortListCorrectness(int list_size, int item_size, uint seed_test = 0, uint seed_hash = 0)
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
            var hash_list = new VeryShortHashList();
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

        public void TestAllVeryShortListCorrectness(int max_list_size, int test_count = 20)
        {
            int hash_fails = 0, hash_count = 0;
            for (; test_count > 0; --test_count)
            {
                Console.WriteLine($"Test no {21 - test_count}");
                for (int list_size = 1; list_size < max_list_size; ++list_size)
                    for (int item_size = list_size - 1; item_size < 1024; ++item_size)
                    {
                        ++hash_count;
                        if (!TestOneVeryShortListCorrectness(list_size, item_size))
                            ++hash_fails;
                    }
            }
            Console.WriteLine($"Hash fails {hash_fails} of {hash_count}");
        }


        public void TestVeryShortListPerformace(int max_list_size)
        {
            int fake = 0;
            Console.WriteLine("Find(list size;ref time ms;hash time ms;)");
            for (int list_size = 1; list_size < max_list_size; ++list_size)
            {
                var hash_test = new HashSet<ushort>();
                while (hash_test.Count < list_size)
                    hash_test.Add((ushort)rand_quick.Next(0, 0xfff));
                var hash_list = new VeryShortHashList();
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
                var hash_list = new VeryShortHashList();
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

        public void TestGraphCompare(int vertex_count, int edge_count)
        {
            Graph graph = Graph.Generate(vertex_count, edge_count);
            Assert(graph.Compare(graph.Clone()));
            Assert(!graph.Compare(Graph.Generate(vertex_count, edge_count)));
            List<int> permutation = Graph.Permutate(vertex_count);
            Graph perm = graph.Permutate( permutation);
            Assert(!graph.Compare(perm));
            Assert(graph.Compare(perm,permutation));
        }

        public void TestGraphCompare()
        {
            for (int c=25;c>0;--c)
            for (int vertex_count = 8; vertex_count < 24; ++vertex_count)
                for (int edge_count = vertex_count * (vertex_count - 1) / 8; edge_count < vertex_count * (vertex_count - 1) / 4; ++edge_count)
                    TestGraphCompare(vertex_count, edge_count);
        }

        public void PrintRandGraphPermutation(int vertex_count, int edge_count)
        {
            Graph graph = Graph.Generate(vertex_count, edge_count);
            List<int> permutation = Graph.Permutate(vertex_count);
            Graph perm = graph.Permutate(permutation);
            Assert(!graph.Compare(perm));
            Assert(graph.Compare( perm, permutation));
            Debug.WriteLine(graph.ToString());
            Debug.WriteLine(perm.ToString());
        }

        public void TestGraphIso(int vertex_count, int edge_count)
        {
            //Debug.WriteLine($"rand seed {RandQuick.Shared.last}");
            Graph graph = Graph.Generate(vertex_count, edge_count);
            List<int> permutation = Graph.Permutate(vertex_count);
            Graph perm = graph.Permutate(permutation);
            //Debug.WriteLine(graph.ToString());
            //Debug.WriteLine(perm.ToString());
            List<int> result = new GraphIso().TryIso(graph, perm);
             Assert(graph.Compare(perm, result));
        }

        public int TestGraphNoIso(int vertex_count, int edge_count)
        {
            //Debug.WriteLine($"rand seed {RandQuick.Shared.last}");
            Graph graph = Graph.Generate(vertex_count, edge_count);
            //Debug.WriteLine(graph.ToString());
            //Debug.WriteLine(perm.ToString());
            List<int> result = new GraphIso().TryIso(graph, Graph.Generate(vertex_count, edge_count));
            return result == null ? 1 : 0;
        }

        public void TestGraphIso()
        {
            for (int vertex_count = 3; vertex_count < 10; ++vertex_count)
                for (int edge_count = vertex_count; edge_count < vertex_count * (vertex_count - 1) / 2; ++edge_count)
                    for (int count = 0; count < 50; ++count)
                    {
                        if (count==0)
                            Console.WriteLine($"{vertex_count} {edge_count}");
                        TestGraphIso(vertex_count, edge_count);
                    }

            for (int vertex_count = 10; vertex_count < 16; ++vertex_count)
                for (int edge_count = vertex_count * (vertex_count - 1) / 4; edge_count < vertex_count * (vertex_count - 1) * 3 / 4; ++edge_count)
                    for (int count = 0; count < 50; ++count)
                    {
                        if (count == 0)
                            Console.WriteLine($"{vertex_count} {edge_count}");
                        TestGraphIso(vertex_count, edge_count);
                    }

            int noiso = 0, total = 0; ;
            for (int vertex_count = 10; vertex_count < 16; ++vertex_count)
                for (int edge_count = vertex_count*(vertex_count-1)/4; edge_count < vertex_count * (vertex_count - 1)*3 /4; ++edge_count)
                    for (int count = 0; count < 50; ++count)
                    {
                        if (count == 0)
                            Console.WriteLine($"{vertex_count} {edge_count}");
                        ++total;
                        noiso += TestGraphNoIso(vertex_count, edge_count);
                    }
            Assert(noiso < total/100);
        }
    }
}
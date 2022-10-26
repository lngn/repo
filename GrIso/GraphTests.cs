using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace GrIso
{
    class GraphTests
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

        public GraphTests(uint rand_seed=1313)
        {
            rand_quick = new RandQuick(rand_seed);
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
            for (int vertex_count = 10; vertex_count < 24; ++vertex_count)
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
            Debug.WriteLine($"rand seed {RandQuick.Shared.last}");
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
            const int repeat_for_graph_size = 10;

            for (int vertex_count = 3; vertex_count < 10; ++vertex_count)
                for (int edge_count = vertex_count; edge_count < vertex_count * (vertex_count - 1) / 2; ++edge_count)
                    for (int count = 0; count < repeat_for_graph_size; ++count)
                    {
                        if (count==0)
                            Console.WriteLine($"{vertex_count} {edge_count}");
                        TestGraphIso(vertex_count, edge_count);
                    }

            for (int vertex_count = 10; vertex_count < 16; ++vertex_count)
                for (int edge_count = vertex_count * (vertex_count - 1) / 4; edge_count < vertex_count * (vertex_count - 1)  / 2 ; ++edge_count)
                    for (int count = 0; count < repeat_for_graph_size; ++count)
                    {
                        if (count == 0)
                            Console.WriteLine($"{vertex_count} {edge_count}");
                        TestGraphIso(vertex_count, edge_count);
                    }

            int noiso = 0, total = 0;
            for (int vertex_count = 10; vertex_count < 16; ++vertex_count)
                for (int edge_count = vertex_count*(vertex_count-1)/4; edge_count < vertex_count * (vertex_count - 1) / 2; ++edge_count)
                    for (int count = 0; count < repeat_for_graph_size; ++count)
                    {
                        if (count == 0)
                            Console.WriteLine($"{vertex_count} {edge_count}");
                        ++total;
                        noiso += TestGraphNoIso(vertex_count, edge_count);
                    }
            Assert(noiso < total/100);
        }

        public static void TestGraphIso(uint rand_seed_graph, uint rand_seed_perm, int vertex_count, int edge_count, bool print)
        {
            if (rand_seed_graph !=0)
                RandQuick.Shared.last = rand_seed_graph;
            Graph graph = Graph.Generate(vertex_count, edge_count);

            if (rand_seed_perm != 0)
                RandQuick.Shared.last = rand_seed_perm;
            var permutation = Graph.Permutate(vertex_count);
            Graph perm = graph.Permutate(permutation);
            var graph_iso = new GraphIso();

            var stopwatch = Stopwatch.StartNew();
            var result = graph_iso.TryIso(graph, perm);
            double t = stopwatch.Elapsed.TotalSeconds;

            if (!graph.Compare(perm, result))
                throw new AbortException("no graph iso");
            Console.WriteLine($"elapsed time {t}");

            if (!print) return;
            Console.WriteLine($"rand_seed_graph={rand_seed_graph} rand_seed_perm={rand_seed_perm } vertex_count={vertex_count } edge_count={edge_count}");
            Console.WriteLine("Some graph");
            Console.WriteLine(graph);
            Console.WriteLine("Other graph");
            Console.WriteLine(perm);
            Console.WriteLine("Permutation");
            Console.WriteLine(string.Join(',', permutation));
        }

        public static void TestOk()
        {
            var tests = new GraphTests();
            tests.TestGraphCompare();
            tests.TestGraphIso();
        }
    }
}
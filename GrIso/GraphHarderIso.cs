using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GrIso
{
    class GraphHarderIso
    {
        static RandQuick rand_quick = RandQuick.Shared;

        public bool Satisfacted { get; set; }
        public delegate void StatusHandler(GraphHarderIso graph_harder_iso);
        public event StatusHandler StatusHandlers;


        public bool TestMode { get; set; }
        public int GraphsCount { get; set; }
        public int EdgesCount { get; set; }
        public int VerticesCount { get; set; }
        public int RankNotImproved { get; set; }
        public int CountNotImproved { get; set; }

        Graph sub_graph;
        List<int> permutation;
        GraphIso graph_iso;
        double graphs_cost;
        Stopwatch stopwatch = Stopwatch.StartNew();

        public int MutationsLimit { get; set; } = int.MaxValue;
        public int ImprovedLimit { get; set; } = int.MaxValue;
        public int MutationsCount { get; private set; }
        public int ImprovedCount { get; private set; }
        public int RemovedCount { get; private set; }
        public double InitialIsoCost { get; private set; }
        public double InitialTimeCost { get; private set; }
        public double CurrentIsoCost { get; private set; }
        public double CurrentTimeCost { get; private set; }

        class GraphIsoCost
        {
            public Graph graph;
            public double iso_cost;
            public double time_cost;
            public int count_not_improved;
        }

        GraphIsoCost[] graphs;

        public void TryHarderIso()
        {
            GenerateGraphs();
            
            while (!Satisfacted && MutationsCount < MutationsLimit && ImprovedCount < ImprovedLimit)
            {
                ++MutationsCount;               
                var (some_graph, other_graph) = SelectGraphs();
                var graph = Mutate(graphs[some_graph].graph, graphs[other_graph].graph);
                
                var (iso_cost, time_cost) = Evaluate(graph);
                if (IsDuplicated(graph, iso_cost) || (iso_cost <= graphs[some_graph].iso_cost && iso_cost <= graphs[other_graph].iso_cost))
                {
                    ++graphs[some_graph].count_not_improved;
                    ++graphs[other_graph].count_not_improved;
                    if (graphs[some_graph].count_not_improved > CountNotImproved && some_graph > RankNotImproved)
                       Replace(some_graph);
                    if (graphs[other_graph].count_not_improved > CountNotImproved && other_graph > RankNotImproved)
                        Replace(other_graph);
                }
                else
                {
                    ++ImprovedCount;
                    graphs[some_graph].count_not_improved = 0;
                    graphs[other_graph].count_not_improved = 0;
                    var graph_index = some_graph > other_graph ? some_graph : other_graph;
                    Replace(graph_index, graph, iso_cost, time_cost);
                }

                if (TestMode)
                    TestHarderIso();
                StatusHandlers?.Invoke(this);
            }
        }


        void GenerateGraphs()
        {
            permutation = Graph.Permutate(VerticesCount);
            sub_graph = new Graph(VerticesCount);
            sub_graph.Add(VerticesCount-1, 0);
            for (int i = 0; i < VerticesCount-1; ++i)
                sub_graph.Add(i, i + 1);
            graph_iso = new GraphIso();

            graphs_cost = 0;
            graphs = new GraphIsoCost[GraphsCount];
            for (int i=0;i<GraphsCount;++i)
            {
                var graph = GenerateGraph();
                var (iso_cost, time_cost) = Evaluate(graph);
                if (!IsDuplicated(graph, iso_cost, graphs, i))
                {
                    graphs[i] = new GraphIsoCost
                    {
                        graph = graph,
                        iso_cost = iso_cost,
                        time_cost = time_cost,
                        count_not_improved = 0
                    };
                    graphs_cost += iso_cost;
                }
                else --i;
            }
            Array.Sort(graphs, (x,y)=>x.iso_cost > y.iso_cost ? -1:1);
            InitialIsoCost = graphs[0].iso_cost;
            InitialTimeCost = graphs[0].time_cost;
            CurrentIsoCost = InitialIsoCost;
            CurrentTimeCost = InitialTimeCost;
        }

        int SelectGraph(int skip_graph, bool skip_cost)
        {
            int graph_index;

            if (skip_cost)
            {
                if (skip_graph >=0)
                {
                    graph_index = rand_quick.Next(GraphsCount - 2);
                    if (graph_index == skip_graph)
                        graph_index = GraphsCount - 1;
                }
                else
                {
                    graph_index = rand_quick.Next(GraphsCount - 1);
                }
            }
            else
            {
                var graphs_cost = this.graphs_cost;
                if (skip_graph >= 0)
                    graphs_cost -= graphs[skip_graph].iso_cost;

                double rand_cost = rand_quick.Next(graphs_cost);
                double acum_cost = 0;
                
                for (graph_index = 0; graph_index < GraphsCount; ++graph_index)
                    if (graph_index != skip_graph)
                    {
                        acum_cost += graphs[graph_index].iso_cost;
                        if (rand_cost < acum_cost)
                            break;
                    }
            }

            return graph_index;
        }

        (int,int) SelectGraphs()
        {
            int some_graph, other_graph;

            switch (MutationsCount % 3)
            {
                case 0:
                    some_graph = SelectGraph(-1, false);
                    other_graph = SelectGraph(some_graph, false);
                    break;
                case 1:
                    some_graph = SelectGraph(-1, false);
                    other_graph = SelectGraph(some_graph, true);
                    break;
                case 2:
                default:
                    some_graph = SelectGraph(-1, true);
                    other_graph = SelectGraph(some_graph, true);
                    break;
            }

            return (some_graph, other_graph);
        }

        Graph GenerateGraph()
        {
            return Graph.Generate(VerticesCount, EdgesCount, sub_graph);
        }

        IEnumerable<Graph.Edge> GetMutationEdges(Graph graph_1, Graph graph_2 )
        {
            foreach (var edge in graph_1.Edges)
                if (!sub_graph.Find(edge))
                    yield return edge;
            foreach (var edge in graph_2.Edges)
                if (!graph_1.Find(edge))
                    yield return edge;
        }

        int CntAvailableEdges(Graph graph_1, Graph graph_2)
        {
            int availabe_edges = EdgesCount - sub_graph.EdgesCount;
            foreach (var edge in graph_2.Edges)
                if (!graph_1.Find(edge))
                    ++availabe_edges;
            return availabe_edges;
        }

        Graph Mutate(Graph graph_1, Graph graph_2)
        {
            int availabe_edges = CntAvailableEdges(graph_1, graph_2);
            int needed_edges = EdgesCount - sub_graph.EdgesCount;
            if (availabe_edges == needed_edges)
                Program.Abort("Identical graphs for mutation!");

            Graph graph = sub_graph.Clone();
            foreach (var edge in GetMutationEdges(graph_1, graph_2))
            {
                if (availabe_edges == needed_edges)
                {
                    graph.Add(edge);
                    availabe_edges -= 1;
                    needed_edges -= 1;
                }
                else
                {
                    availabe_edges -= 1;
                    if (rand_quick.Next(availabe_edges) < needed_edges)
                    {
                        graph.Add(edge);
                        needed_edges -= 1;
                        if (needed_edges == 0)
                            break;
                    }
                }
            }

            if (graph.EdgesCount != EdgesCount)
                Program.Abort("Mutation has invalid number of edges!");

            return graph;
        }

        (double,double) Evaluate(Graph some_graph)
        {
            stopwatch.Restart();
            var other_graph = some_graph.Permutate(permutation);
            var permutation_iso = graph_iso.TryIso(some_graph, other_graph);
            if (permutation_iso == null || !some_graph.Compare(other_graph, permutation_iso))
                Program.Abort("Mutation is not permutation iso!");
            return (graph_iso.GraphIsoCost, stopwatch.Elapsed.TotalMilliseconds);
        }

        bool IsDuplicated(Graph graph, double iso_cost)
        {
            for (int i = 0; i < GraphsCount; ++i)
                if (graphs[i].iso_cost == iso_cost && graphs[i].graph.Compare(graph))
                    return true;
            return false;
        }

        bool IsDuplicated(Graph graph, double iso_cost, GraphIsoCost[] mutations, int mutations_count)
        {
            for (int i = 0; i < mutations_count; ++i)
                if (mutations[i].iso_cost == iso_cost && mutations[i].graph.Compare(graph))
                    return true;
            return false;
        }

        void Replace(int graph_index)
        {
            Assert(graph_index != 0);

            ++RemovedCount;
            var graph = GenerateGraph();
            var (iso_cost, time_cost) = Evaluate(graph);
            if (!IsDuplicated(graph, iso_cost))
                Replace(graph_index, graph, iso_cost, time_cost);
        }

        void Replace(int graph_index, Graph graph, double iso_cost, double time_cost)
        {
            GraphIsoCost graph_iso_cost = graphs[graph_index];
            graphs_cost += iso_cost - graph_iso_cost.iso_cost;
            graph_iso_cost.graph = graph;
            graph_iso_cost.iso_cost = iso_cost;
            graph_iso_cost.time_cost = time_cost;
            graph_iso_cost.count_not_improved = 0;

            while (graph_index > 0 && iso_cost > graphs[graph_index-1].iso_cost)
            {
                graphs[graph_index] = graphs[graph_index - 1];
                --graph_index;
            }
            while (graph_index < GraphsCount -1 && iso_cost < graphs[graph_index + 1].iso_cost)
            {
                graphs[graph_index] = graphs[graph_index + 1];
                ++graph_index;
            }
            graphs[graph_index] = graph_iso_cost;
            if (graph_index == 0)
            {
                CurrentIsoCost = graphs[0].iso_cost;
                CurrentTimeCost = graphs[0].time_cost;
            }
        }

        void Assert(bool b)
        {
            if (!b)
            {
                Console.WriteLine("Fail");
                throw new AbortException("Assert");
            }
        }

        public void TestHarderIso()
        {
            Console.WriteLine(MutationsCount);
            var mutations_count = MutationsCount;

            double test_graphs_cost = 0;
            for (var graphIndex = 0; graphIndex < GraphsCount; ++graphIndex)
            {
                Assert(graphIndex == 0 || graphs[graphIndex].iso_cost <= graphs[graphIndex - 1].iso_cost);
                var test_graph_cost = graphs[graphIndex];
                Assert(test_graph_cost.graph.EdgesCount == EdgesCount);
                var permutation_graph = test_graph_cost.graph.Permutate(permutation);
                var test_graph_iso = new GraphIso();
                var permutation_iso = test_graph_iso.TryIso(test_graph_cost.graph, permutation_graph);
                Assert(permutation_iso != null && test_graph_cost.graph.Compare(permutation_graph, permutation_iso));
                Assert(test_graph_cost.iso_cost == test_graph_iso.GraphIsoCost);
                test_graphs_cost += test_graph_iso.GraphIsoCost;
            }
            Assert(test_graphs_cost == graphs_cost);

            // Test graph selection. For each selection kind (3 type x 2 graphs) count how many each graph was selected. 
            var test_select_count = 100*GraphsCount;
            var test_select_value = new double[6, GraphsCount];
            const double test_accepted_deviation = 0.15;
            
            for (var i=0;i< 3*test_select_count;++i)
            {
                ++MutationsCount;
                var (some_graph, other_graph) = SelectGraphs();
                Assert(some_graph != other_graph);
                var test_select_type = MutationsCount % 3;
                ++test_select_value[2* test_select_type, some_graph];
                ++test_select_value[2* test_select_type + 1, other_graph];
            }

            // Test graph selection. For each selection kind (3 type x 2 graphs) estimate how many each graph should be selected. 
            var test_select_expected = new double[6, GraphsCount];
            for (var i=0;i<6;++i)
            {
                if (i==0 || i==2)
                {
                    for (var i1 = 0; i1 < GraphsCount; ++i1)
                    {
                        double rand_first = graphs[i1].iso_cost / graphs_cost; ;
                        test_select_expected[i, i1] = test_select_count * rand_first;
                    }
                }
                else if (i == 1)
                {
                    for (var i1 = 0; i1 < GraphsCount; ++i1)
                    {
                        double rand_second = 0;
                        for (var i2 = 0; i2 < GraphsCount; ++i2)
                            if (i1 != i2)
                                rand_second += graphs[i2].iso_cost / graphs_cost * graphs[i1].iso_cost / (graphs_cost - graphs[i2].iso_cost);
                        Assert(rand_second <= 1);
                        test_select_expected[i, i1] = test_select_count * rand_second;
                    }
                }
                else if (i==3)
                {
                    for (var i1 = 0; i1 < GraphsCount; ++i1)
                    {
                        double rand_second = (graphs_cost - graphs[i1].iso_cost) / graphs_cost / (GraphsCount - 1);
                        Assert(rand_second <= 1);
                        test_select_expected[i, i1] = test_select_count * rand_second;
                    }
                }
                else
                {
                    double rand_value = 1.0 / GraphsCount;
                    for (var i1 = 0; i1 < GraphsCount; ++i1)
                    {
                        test_select_expected[i, i1] = test_select_count *rand_value;
                    }
                }
            }

            // Test graph selection. For each graph test deviation is accepted(test_accepted_deviation)
            for (var i1 = 0; i1< 6; ++i1)
            {
                for (var i2 = 0; i2 < GraphsCount; ++i2)
                    Assert( Math.Abs(test_select_expected[i1, i2] - test_select_value[i1, i2]) < test_accepted_deviation * test_select_count);
            }

            // Test mutation includes each edge with equal channce and mutated graphs are actually different
            var test_graphs_count = 5;
            var test_mutations_count = 300;
            
            for ( var i1=0;i1<test_graphs_count; ++i1)
            {
                ++MutationsCount;
                var selected_graphs = SelectGraphs();
                var some_graph = graphs[selected_graphs.Item1].graph;
                var other_graph = graphs[selected_graphs.Item2].graph;
                int availabe_edges = CntAvailableEdges(some_graph, other_graph);
                int needed_edges = EdgesCount - sub_graph.EdgesCount;

                var graphes = new HashSet<Graph> {  };
                var edges_counters = new Dictionary<Graph.Edge, int> { };
                foreach (var edge in GetMutationEdges(some_graph, other_graph))
                    edges_counters[edge] = 0;
                for (var i2 = 0; i2 < test_mutations_count; ++i2)
                {
                    var graph = Mutate(some_graph, other_graph);
                    graphes.Add(graph);
                    foreach (var edge in graph.Edges)
                        if (!sub_graph.Find(edge))
                            ++edges_counters[edge];
                }

                // Test each edge was selected with accepted chance
                double test_edges_count = test_mutations_count * needed_edges / availabe_edges;
                var test_accepted_count = test_accepted_deviation * test_mutations_count;
                foreach (var edges_counter in edges_counters)
                    Assert(Math.Abs(test_edges_count - edges_counter.Value) < test_accepted_count);

                test_accepted_count = test_accepted_deviation * test_mutations_count;
                Assert(graphes.Count > test_accepted_count);
            }

            MutationsCount = mutations_count;
        }
    }
}

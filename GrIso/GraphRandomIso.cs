using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GrIso
{
    class GraphRandomIso
    {
        public bool Satisfacted { get; set; }
        public delegate void StatusHandler(GraphRandomIso graph_random_iso);
        public event StatusHandler StatusHandlers;

        public int EdgesCount { get; set; }
        public int VerticesCount { get; set; }

        public int SubEdgesCount { get; set; }
        public int SubVerticesCount { get; set; }

        public int RandomLimit { get; set; } = int.MaxValue;
        public int ImprovedLimit { get; set; } = int.MaxValue;
        public int InitialLimit { get; set; } = 10;
        public int RandomCount { get; private set; }
        public int ImprovedCount { get; private set; }

        public double InitialIsoCost { get; private set; }
        public double InitialTimeCost { get; private set; }
        public double CurrentIsoCost { get; private set; }
        public double CurrentTimeCost { get; private set; }

        List<int> permutation;
        Graph graph;
        Stopwatch stopwatch = Stopwatch.StartNew();
        GraphIso graph_iso;

        public void TryRandomIso()
        {
            graph_iso = new GraphIso();
            permutation = Graph.Permutate(VerticesCount);
            
            while (!Satisfacted && RandomCount < RandomLimit && ImprovedCount < ImprovedLimit)
            {
                ++RandomCount;
                var (some_graph, iso_cost, time_cost) = RandomGraph();
                if (iso_cost > CurrentIsoCost)
                {
                    ++ImprovedCount;
                    graph = some_graph;
                    CurrentIsoCost = iso_cost;
                    CurrentTimeCost = time_cost;
                }
                if (RandomCount == InitialLimit)
                {
                    InitialIsoCost = CurrentIsoCost;
                    InitialTimeCost = CurrentTimeCost;
                    ImprovedCount = 0;
                }
                StatusHandlers?.Invoke(this);
            }
        }

        (Graph, double, double) RandomGraph()
        {
            Graph some_graph;
            if (SubVerticesCount > 0 && SubEdgesCount > 0)
                some_graph = Graph.Generate(VerticesCount, EdgesCount, SubVerticesCount, SubEdgesCount);
            else
                some_graph = Graph.Generate(VerticesCount, EdgesCount);

            stopwatch.Restart();
            var other_graph = some_graph.Permutate(permutation);
            var permutation_iso = graph_iso.TryIso(some_graph, other_graph);
            if (permutation_iso == null || !some_graph.Compare(other_graph, permutation_iso))
                Program.Abort("Mutation is not permutation iso!");
            return (graph, graph_iso.GraphIsoCost, stopwatch.Elapsed.TotalMilliseconds);
        }
    }
}

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


        int graphs_count;
        int edges_count;
        int verticies_count;
        Graph sub_graph;
        List<int> permutation;
        GraphIso graph_iso;
        int mutations_count;
        double graphs_cost;
        int rank_not_improved;
        int count_not_improved;

        class GraphIsoCost
        {
            public Graph graph;
            public double iso_cost;
            public int count_not_improved;
        }

        GraphIsoCost[] graphs;

        public void TryHarderIso()
        {
            GenerateGraphs();

            while (!Satisfacted)
            {

                ++mutations_count;
                var (some_graph, other_graph) = SelectGraphs();
                var graph = Mutate(graphs[some_graph].graph, graphs[other_graph].graph);
                var iso_cost = Evaluate(graph);
                if (IsDuplicated(graph, iso_cost) || (iso_cost < graphs[some_graph].iso_cost && iso_cost < graphs[other_graph].iso_cost))
                {
                    ++graphs[some_graph].count_not_improved;
                    ++graphs[other_graph].count_not_improved;
                    if (graphs[some_graph].count_not_improved > count_not_improved && some_graph > rank_not_improved)
                       Replace(some_graph);
                    if (graphs[other_graph].count_not_improved > count_not_improved && other_graph > rank_not_improved)
                        Replace(other_graph);
                }
                else
                {
                    graphs[some_graph].count_not_improved = 0;
                    graphs[other_graph].count_not_improved = 0;
                    Replace(other_graph, graph, iso_cost);
                }

                StatusHandlers(this);
            }
        }


        void GenerateGraphs()
        {
            graphs = new GraphIsoCost[graphs_count];
            for (int i=0;i<graphs_count;++i)
            {
                var graph = GenerateGraph();
                var iso_cost = Evaluate(graph);
                if (!IsDuplicated(graph, iso_cost, graphs, i))
                    graphs[i] = new GraphIsoCost { 
                        graph = graph, iso_cost = iso_cost, count_not_improved = 0
                    };
                else --i;
            }
            Array.Sort(graphs, (x,y)=>x.iso_cost > y.iso_cost ? -1:1);
        }

        int SelectGraph(int skip_graph, bool skip_cost)
        {
            int graph_index;

            if (skip_cost)
            {
                if (skip_graph >=0)
                {
                    graph_index = rand_quick.Next(graphs_count - 2);
                    if (graph_index >= skip_graph)
                        ++graph_index;
                }
                else
                {
                    graph_index = rand_quick.Next(graphs_count - 1);
                }
            }
            else
            {
                var graphs_cost = this.graphs_cost;
                if (skip_graph >= 0)
                    graphs_cost -= graphs[skip_graph].iso_cost;

                double rand_cost = rand_quick.Next(graphs_cost);
                double acum_cost = 0;
                
                for (graph_index = 0; graph_index < graphs_count; ++graph_index)
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

            switch (mutations_count % 3)
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
            if (some_graph < other_graph)
                return (some_graph, other_graph);
            else
                return (other_graph, some_graph);
        }

        Graph GenerateGraph()
        {
            return Graph.Generate(verticies_count, edges_count, sub_graph);
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

        Graph Mutate(Graph graph_1, Graph graph_2)
        {
            GraphIsoCost mutation = new GraphIsoCost();
            
            int availabe_edges = edges_count;
            foreach (var edge in graph_2.Edges)
                if (!graph_1.Find(edge))
                    ++availabe_edges;

            int needed_edges = edges_count - sub_graph.EdgesCount;
            if (availabe_edges == needed_edges)
                Program.Abort("Identical graphs for mutation!");

            Graph graph = new Graph(verticies_count);
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

            if (graph.EdgesCount != edges_count)
                Program.Abort("Mutation has invalid number of edges!");

            return graph;
        }

        double Evaluate(Graph some_graph)
        {
            var other_graph = some_graph.Permutate(permutation);
            var permutation_iso = graph_iso.TryIso(some_graph, other_graph);
            if (permutation_iso == null || !some_graph.Compare(other_graph, permutation_iso))
                Program.Abort("Mutation is not permutation iso!");
            return graph_iso.GraphIsoCost;
        }

        bool IsDuplicated(Graph graph, double iso_cost)
        {
            for (int i = 0; i < graphs_count; ++i)
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
            var graph = GenerateGraph();
            var iso_cost = Evaluate(graph);
            if (!IsDuplicated(graph, iso_cost))
                Replace(graph_index, graph, iso_cost);
        }

        void Replace(int graph_index, Graph graph, double iso_cost)
        {
            GraphIsoCost graph_iso_cost = graphs[graph_index];
            graphs_cost += iso_cost - graph_iso_cost.iso_cost;
            graph_iso_cost.graph = graph;
            graph_iso_cost.iso_cost = iso_cost;
            graph_iso_cost.count_not_improved = 0;

            while (graph_index >0 && iso_cost > graphs[graph_index-1].iso_cost)
            {
                graphs[graph_index] = graphs[graph_index - 1];
                --graph_index;
            }
            while (graph_index < graphs_count -1 && iso_cost < graphs[graph_index + 1].iso_cost)
            {
                graphs[graph_index] = graphs[graph_index + 1];
                ++graph_index;
            }
            graphs[graph_index] = graph_iso_cost;
        }
    }
}

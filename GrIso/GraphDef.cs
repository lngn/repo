
using System;
using System.Collections.Generic;
using System.Text;

namespace GrIso
{

    class Graph : List<HashSet<int>>
    {

        static RandQuick rand_quick = RandQuick.Shared;
        const int None = -1;

        public Graph(int vertex_count)
            : base(vertex_count)
        {
            for (int i = 0; i < vertex_count; ++i)
                Add(new HashSet<int>());
        }


        public Graph Clone()
        {
            var clone = new Graph(Count);
            for (int i = 0; i < Count; ++i)
                clone[i] = new HashSet<int>(this[i]);
            clone.edges_count = edges_count;
            return clone;
        }

        private int edges_count;
        public int EdgesCount => edges_count;

        public struct Edge
        {
            public int some_vertex;
            public int other_vertex;
        }

        public IEnumerable<Edge> Edges {
            get 
            {
                Edge edge= new Edge { };
                for (var some_vertex = 0; some_vertex < Count; ++some_vertex)
                    foreach (var other_vertex in this[some_vertex])
                        if (some_vertex < other_vertex)
                        {
                            edge.some_vertex = some_vertex;
                            edge.other_vertex = other_vertex;
                            yield return edge;
                        }
            }
            set
            {
                foreach (var edge in value)
                {
                    this[edge.some_vertex].Add(edge.other_vertex);
                    this[edge.other_vertex].Add(edge.some_vertex);
                }
            }
        }

        public bool Find(Edge edge)
        {
            return this[edge.some_vertex].Contains(edge.other_vertex);
        }

        public void Add(Edge edge)
        {
            Add(edge.some_vertex, edge.other_vertex);
        }

        public void Remove(Edge edge)
        {
            Remove(edge.some_vertex, edge.other_vertex);
        }

        public Graph Clone(Graph graph)
        {
            for (int vertex_1 = 0; vertex_1 < Count; ++vertex_1)
                foreach (int vertex_2 in this[vertex_1])
                    graph.Add(vertex_1, vertex_2);
            return graph;
        }

        public bool Find(int some_vertex, int other_vertex)
        {
            return this[some_vertex].Contains(other_vertex);
        }

        public void Add(int some_vertex, int other_vertex)
        {
            if (!this[some_vertex].Contains(other_vertex))
            {
                this[some_vertex].Add(other_vertex);
                this[other_vertex].Add(some_vertex);
                ++edges_count;
            }
        }

        public void Remove(int some_vertex, int other_vertex)
        {
            if (this[some_vertex].Contains(other_vertex))
            {
                this[some_vertex].Remove(other_vertex);
                this[other_vertex].Remove(some_vertex);
                --edges_count;
            }
        }

        public override bool Equals(object obj)
        {
            return Compare(obj as Graph);
        }

        public override int GetHashCode()
        {
            var hash_quick = new HashQuick();
            foreach (var vertex in this )
                hash_quick.Hash(vertex.Count);
            return hash_quick.Hash();
        }

        public bool Compare(Graph graph)
        {
            if (this == graph)
                return true;
            if (this == null || graph == null)
                return false;
            if (Count != graph.Count)
                return false;

            for (int count_vertex = Count, some_vertex = 0; some_vertex < count_vertex; ++some_vertex)
                if (graph[some_vertex].Count != this[some_vertex].Count)
                    return false;

            for (int count_vertex = Count, some_vertex = 0; some_vertex < count_vertex; ++some_vertex)
                foreach (int other_vertex in this[some_vertex])
                    graph[some_vertex].Contains(other_vertex);

                return true;
        }
        
        public override string ToString()
        {
            return ToString(", ", "\r\n");
        }

        public string ToString(string edge_separator = ", ", string vertex_separator = "\r\n")
        {
            var sb = new StringBuilder();
            for (int vertex_1 = 0; vertex_1 < Count; ++vertex_1)
            {
                var vertex_2 = this[vertex_1].GetEnumerator();
                bool is_next = vertex_2.MoveNext();
                while (is_next)
                {
                    sb.Append($"[{vertex_1}{edge_separator}{vertex_2.Current}]");
                    is_next = vertex_2.MoveNext();
                    sb.Append(is_next ?  edge_separator : vertex_separator);
                }
            }
            return sb.ToString();
        }

        public static List<int> Permutate(int vertex_count)
        {
            // Fisher-Yates shuffles
            var permutation = new List<int>(vertex_count);
            for (int i = 0; i < vertex_count; ++i)
                permutation.Add(i);
            for (int i1=0; i1< vertex_count - 1; ++i1)
            {
                int i2 = rand_quick.Next(i1, vertex_count -1 );
                int i3 = permutation[i1];
                permutation[i1] = permutation[i2];
                permutation[i2] = i3;
            }

            return permutation;
        }

        public static Graph GenerateTree(int vertex_count)
        {
            var graph = new Graph(vertex_count);
            //  create random tree to make graph connected
            var subtree_vertex = new List<int>();
            // list all vertex
            for (int i = 0; i < vertex_count; ++i)
                subtree_vertex.Add(i);
            // append to connected subtree random edges
            for (int i = 1; i < vertex_count; ++i)
            {
                // vertex in subtree
                int i1 = rand_quick.Next(0, i - 1);
                // vertex not in subtree
                int i2 = rand_quick.Next(i, vertex_count - 1);
                // append new edge
                graph.Add(subtree_vertex[i1], subtree_vertex[i2]);
                // swap selected vertex to keep booking already selected vertex in connected subtree
                i1 = subtree_vertex[i];
                subtree_vertex[i] = subtree_vertex[i2];
                subtree_vertex[i2] = i1;
            }

            return graph;
        }

        // Generate random connected graph.
        public static Graph Generate(int vertex_count, int edge_count)
        {
            return Generate(vertex_count, edge_count, GenerateTree(vertex_count));
        }

        public static Graph Generate(int vertex_count, int edge_count, int subvertex_count, int subedge_count)
        {
            var graph = GenerateTree(vertex_count);

            var subgraph = Graph.Generate(subvertex_count, subedge_count);
            var permutation = new int[vertex_count];
            for (var i = 0; i < vertex_count; ++i)
                permutation[i] = i;

           while(graph.EdgesCount < edge_count)
            {
                for ( var i1=0;i1<subvertex_count;++i1)
                {
                    var i2 = rand_quick.Next(i1, vertex_count - 1);
                    var i3 = permutation[i1];
                    permutation[i1] = permutation[i2];
                    permutation[i2] = i3;
                }
                foreach (var edge in subgraph.Edges)
                {
                    graph.Add(permutation[edge.some_vertex], permutation[edge.other_vertex]);
                    if (graph.EdgesCount >= edge_count)
                        break;
                }
            }

            return graph;
        }

        // Generate random connected graph.
        public static Graph Generate(int vertex_count, int edge_count, Graph subgraph)
        {
            if (edge_count < vertex_count - 1)
                Program.Abort("too little edge - only connected graph");
            if (edge_count > vertex_count * (vertex_count - 1) / 2)
                Program.Abort("too many edge - symetric graph");
            if (edge_count < subgraph.Count)
                Program.Abort("too big subgraph");

            var graph = subgraph.Clone();
            
            // not too dense graph rand included edge with 1/2 chance of not already used
            if (edge_count < vertex_count * (vertex_count - 1) / 4)
            {    
                while ( graph.edges_count < edge_count)
                {
                    int i1 = rand_quick.Next(0, vertex_count - 1);
                    int i2 = rand_quick.Next(0, vertex_count - 1);
                    if (i1 != i2 && !graph.Find(i1, i2))
                        graph.Add(i1, i2);
                }

                return graph;
            }

            // too dense graph rand excluded edge with 1/2 chance of not already used
            var excluded = new HashSet<(int, int)>();
            for (int i = vertex_count - 1; i < edge_count; ++i)
            {
                int i1 = rand_quick.Next(0, vertex_count - 1);
                int i2 = rand_quick.Next(0, vertex_count - 1);
                if (i2 < i1) { int i3 = i1; i1 = i2; i2 = i3; }
                if (i1 != i2 && !graph.Find(i1, i2) && !excluded.Contains((i1,i2)))
                    excluded.Add( (i1, i2) );
            }

            for (int i1 = 0; i1 < vertex_count; ++i1) 
                for (int i2 = i1 + 1; i2 < vertex_count; ++i2)
            {
                    if (!excluded.Contains((i1, i2)))
                        graph.Add(i1, i2);
            }

            return graph;
        }

        public bool Compare(Graph graph, List<int> permutation)
        {
            if (Count != graph.Count)
                return false;
            for (int i = 0; i < Count; ++i)
                if (this[i].Count != graph[permutation[i]].Count)
                    return false;

            for (int i1 = 0,  c1 = Count; i1 < c1; ++i1)
            {
                var v = this[i1];
                foreach (int i2 in v)
                {
                    if (!graph.Find(permutation[i1], permutation[i2]))
                        return false;
                }
            }

            return true;
        }

        public Graph Permutate(List<int> permutation)
        {
            var graph = new Graph(Count);
            for (int i1 = 0; i1 < Count; ++i1)
            {
                foreach (int i2 in this[i1])
                    graph.Add(permutation[i1], permutation[i2]);
            }

            return graph;
        }
    }}

using System;
using System.Collections.Generic;
using System.Text;

namespace GrIso
{
    
    class Graph: List<HashSet<int>>    
    {
        
        static RandQuick rand_quick = RandQuick.Shared;
        const int None = -1;

        public Graph(int vertex_count)
            :base(vertex_count)
        {
            for (int i = 0; i < vertex_count; ++i)
                Add(new HashSet<int>());
        }
        public void Add(int some_vertex, int other_vertex)
        {
            this[some_vertex].Add(other_vertex);
            this[other_vertex].Add(some_vertex);
        }
        
        public Graph Clone()
        {
            var clone = new Graph(Count);
            for (int i = 0; i < Count; ++i)
                clone[i] = new HashSet<int>(this[i]);
            return clone;
        }
        public bool Find(int some_vertex, int other_vertex)
        {
            return this[some_vertex].Contains(other_vertex);
        }

        public bool Compare(Graph graph)
        {
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
                int i2 = rand_quick.Next(i1, vertex_count - i1);
                int i3 = permutation[i1];
                permutation[i1] = permutation[i2];
                permutation[i2] = i3;
            }

            return permutation;
        }

        // Generate random connected graph.
        public static Graph Generate(int vertex_count, int edge_count)
        {
            if (edge_count < vertex_count - 1)
                Program.Abort("too little edge - only connected graph");
            if (edge_count > vertex_count * (vertex_count - 1) / 2)
                Program.Abort("too many edge - symetric graph");

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

            // not too dense graph rand included edge with 1/2 chance of not already used
            if (edge_count < vertex_count * (vertex_count - 1) / 4)
            {    
                for (int i = vertex_count - 1; i < edge_count; ++i)
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
    };
}

using System;
using System.Collections.Generic;
using System.Text;

namespace GrIso
{
    class AbortException: Exception
    {
        public AbortException(string message) : base(message) { }
    }

    class GraphVertex : ShortHashList   
    {
        public new GraphVertex Clone()
        {
            var clone = new GraphVertex();
            base.Clone(clone);
            return clone;
        }

        private new void Append(ushort vertex) { }
        public void Append(int vertex)
        {
            if (vertex >= None)
                throw new AbortException("Vertex number out of range");
            base.Append((ushort)vertex);
        }
        private new ushort Next(ushort vertex) { return None;  }
        public int Next(int vertex)
        {
            return base.Next((ushort)vertex);
        }
        public new int First()
        {
            return base.First();
        }
        private new bool Find(ushort vertex) { return false;  }
        public bool Find(int item)
        {
            uint hash_val = (uint)item;
            int hash_index = (int)((hash_val * hash_mul + hash_add) >> hash_shift);
            return item == hash_list[hash_index];
        }
    }

    class Graph: List<GraphVertex>    
    {    
        public Graph(int vertex_count)
            :base(vertex_count)
        {
            for (int i = 0; i < vertex_count; ++i)
                this.Add(new GraphVertex());
        }
        public void Append(int some_vertex, int other_vertex)
        {
            this[some_vertex].Append(other_vertex);
            this[other_vertex].Append(some_vertex);
        }
        public bool Hash()
        {
            for (int i = 0; i < Count; ++i)
            {
                if (!this[i].Hash())
                    return false;
            }
            return true;
        }
        public Graph Clone()
        {
            var clone = new Graph(Count);
            for (int i = 0; i < Count; ++i)
                clone[i] = this[i].Clone();
            return clone;
        }
        public bool Find(int some_vertex, int other_vertex)
        {
            return this[some_vertex].Find(other_vertex);
        }
        public bool Compare(Graph graph)
        {
            if (Count != graph.Count)
                return false;
            for (int i = 0; i < Count; ++i)
                if (graph[i].Count() != this[i].Count())
                    return false;

            for (int i1 = 0; i1 < Count; ++i1)
                for (int i2 = this[i1].First(); i2 != GraphVertex.None; i2 = this[i1].Next(i2))
                    if (!graph.Find(i1, i2))
                        return false;

                return true;
        }
        
        public override string ToString()
        {
            var sb = new StringBuilder();
            sb.Append("[");
            int i1 = 0, i2;
            while (true)
            {
                var v = this[i1];
                i2 = v.First();
                while (true)
                {
                    sb.Append($"[{i1},{i2}]");
                    i2 = v.Next(i2);
                    if (i2 == GraphVertex.None)
                        break;
                    sb.Append(",");
                }
                ++i1;
                if (i1 == Count)
                    break;
                sb.Append("]");
            }
            return sb.ToString();
        }
    };

    class GraphFun
    {
        static void Abort(string message) { throw new AbortException(message); }
        RandQuick rand_quick;

        public GraphFun(uint rand_seed)
        {
            rand_quick = new RandQuick(rand_seed);
        }

        // Generate random connected graph.
        public Graph Generate(int vertex_count, int edge_count)
        {
            if (edge_count < vertex_count - 1)
                Abort("too less edge - only connected graph");
            if (edge_count >= vertex_count*( vertex_count - 1))
                Abort("too many edge - symetric graph");

            /* Explicite added and removed edges depending on bool flag. 
             * For dense graph filled only removed edges excepts those have been choosen to make connected graph. For not dense graph is simpler.
             * This gives 1/2 chance of add a random edge.
             */
            var edges = new HashSet<(int, int,bool)>(); 

            var graph = new Graph(vertex_count);
            //  make graph connected
            var permutation = new List<int>();
            // list all vertex
            for (int i = 0; i < vertex_count; ++i)
                permutation.Add(i);
            // append to connected subtree random edges
            for (int i=1;i<vertex_count;++i)
            {
                // vertex in subtree
                int i1 = rand_quick.Next(0, i-1);
                // vertex not in subtree
                int i2 = rand_quick.Next(i, vertex_count - 1);
                // append new edge
                if (permutation[i1] < permutation[i2])
                    edges.Add((permutation[i1], permutation[i2], true));
                else
                    edges.Add((permutation[i2], permutation[i1], true));

                // swap selected vertex to keep booking already selected vertex in connected subtree
                i1 = permutation[i];
                permutation[i] = permutation[i2];
                permutation[i2] = i1;
            }

            if (edge_count > vertex_count*(vertex_count-1)/4)
            {
                for (int i = vertex_count - 1; i < vertex_count*(vertex_count-1)/2-edge_count;++i)
                {
                    int i1 = rand_quick.Next(0, vertex_count - 1);
                    int i2 = rand_quick.Next(0, vertex_count - 1);
                    if (i2 < i1) { int i3 = i1; i1 = i2; i2 = i3; }
                    if (i1 != i2 && !edges.Contains((i1, i2, true)) && !edges.Contains((i1,i2 ,false)))
                        edges.Add((i1, i2, false));
                }
                for (int i1 = 0; i1 < vertex_count; ++i1)
                    for (int i2 = i1+1; i2 < vertex_count; ++i2)
                        if (edges.Contains((i1, i2,true)) || !edges.Contains((i1, i2, false)))
                            graph.Append((ushort)i1,(ushort) i2);
            }
            else
            {
                for (int i = vertex_count - 1; i < edge_count; ++i)
                {
                    int i1 = rand_quick.Next(0, vertex_count - 1);
                    int i2 = rand_quick.Next(0, vertex_count - 1);
                    if (i2<i1)  { int i3 = i1;i1 = i2;i2 = i3; }
                    if (i1 != i2 && !edges.Contains((i1,i2, true)))
                        edges.Add((i1, i2, true));
                }

                foreach (var edge in edges)
                    graph.Append(edge.Item1, edge.Item2);                
            }

            if (!graph.Hash())
                Abort("Cannot make short hash");
            return graph;
        }        

        public List<int> Permutate(int vertex_count)
        {
            var permutation = new List<int>(vertex_count);
            for (int i = 0; i < vertex_count; ++i)
                permutation.Add(i);
            for (int repeat=2*vertex_count;repeat>0;--repeat)
            {
                int i1 = rand_quick.Next(0, vertex_count - 1);
                int i2 = rand_quick.Next(0, vertex_count - 1);
                int i3 = permutation[i1];
                permutation[i1] = permutation[i2];
                permutation[i2] = i3;
            }

            return permutation;
        }

        public bool Compare(Graph some_graph, Graph other_graph, List<int> permutation)
        {
            if (some_graph.Count != other_graph.Count)
                return false;
            for (int i = 0; i < some_graph.Count; ++i)
                if (some_graph[i].Count() != other_graph[permutation[i]].Count())
                    return false;

            for (int i1 = 0; i1 < some_graph.Count; ++i1)
                for (int i2 = some_graph[i1].First(); i2 != GraphVertex.None; i2 = some_graph[i1].Next(i2))
                    if (!other_graph.Find(permutation[i1], permutation[i2]))
                        return false;

            return true;
        }

        public Graph Permutate(Graph some_graph, List<int> permutation)
        {
            var perm_graph = new Graph(some_graph.Count);
            for (int i1 = 0; i1 < some_graph.Count; ++i1)
                for (int i2 = some_graph[i1].First(); i2 != GraphVertex.None; i2 = some_graph[i1].Next(i2))
                    if (i1 < i2)
                        perm_graph.Append(permutation[i1], permutation[i2]);
            if (!perm_graph.Hash())
                Abort("Cannot make short hash");
            return perm_graph;
        }
    }
}

using System;
using System.Collections.Generic;


namespace GrIso
{
    class AbortException: Exception
    {
        public AbortException(string message) : base(message) { }
    }

    class GraphVertex : ShortHashList    {    }
    class Graph: List<GraphVertex>    
    {    
        public Graph(int vertex_count)
            :base(vertex_count)
        {
            for (int i = 0; i < vertex_count; ++i)
                this[i] = new GraphVertex();
        }
        public void Append(ushort some_vertex, ushort other_vertex)
        {
            this[some_vertex].Append(other_vertex);
            this[other_vertex].Append(some_vertex);
        }
        public bool Hash()
        {
            for (int i = 0; i < Count; ++i)
                if (!this[i].Hash())
                    return false;
            return true;
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
            for (int i = 0; i < vertex_count; ++i)
                permutation.Add(i);
            for (int i=1;i<vertex_count;++i)
            {
                int i1 = rand_quick.Next(0, i-1);
                int i2 = rand_quick.Next(i, vertex_count - 1);
                edges.Add((i1, i2, true));
                edges.Add((i2, i1, true));
                int i3 = permutation[i1];
                permutation[i1] = permutation[i2];
                permutation[i2] = i3;
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
                    graph.Append((ushort)edge.Item1, (ushort)edge.Item2);

                if (!graph.Hash())
                    Abort("Cannot make short hash");
            }

            return graph;
        }        

        List<int> Permutate(int vertex_count)
        {
            var permutation = new List<int>(vertex_count);
            for (int i = 0; i < vertex_count; ++i)
                permutation[i] = i;
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

        Graph Permutate(Graph graph)
        {
            var permutaion = Permutate(graph.VertexCount);
        }
    }
}

using System;
using System.Collections.Generic;

namespace GrIso
{
    

    

    class GraphVertex : ShortHashList
    {

    }

    struct Vertex
    {
        List<ushort> edges_list;
        List<ushort> edges_hash;
        uint hash_shift;
        uint hash_mul;
        uint hash_add;
    }

    class Graph: List<Vertex>
    {

    };

    class GraphFun
    {
        RandQuick rand_quick;

        public GraphFun(uint rand_seed)
        {
            rand_quick = new RandQuick(rand_seed);
        }

        public Graph Generate(int vertex_count, int edge_count)
        {
            return null;

            var edges = new HashSet<(int, int)>();

            //  make graph connected
            var vertex = new List<int>();
            for (int i = 0; i < vertex_count; ++i)
                vertex.Add(i);
            for (int i=1;i<vertex_count;++i)
            {
                int i1 = rand_quick.Next(0, i-1);
                int i2 = rand_quick.Next(i, vertex_count - 1);
                edges.Add((i1, i2));
                edges.Add((i2, i1));
                int i3 = vertex[i1];
                vertex[i1] = vertex[i2];
                vertex[i2] = i3;
            }

            if (edge_count > vertex_count*(vertex_count-1)/4)
            {

            }
            else
            {
                for (int i = vertex_count - 1; i < edge_count; ++i)
                {
                    int i1 = rand_quick.Next(0, vertex_count - 1);
                    int i2 = rand_quick.Next(i, vertex_count - 1);
                    if (!edges.Contains((i1,i2)))
                    {
                        edges.Add((i1, i2));
                        edges.Add((i2, i1));
                    }
                }
            }
            
        }        
    }
}
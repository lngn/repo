
#pragma once
#include "ShortHashList.h"
#include <exception>
#include <set>
#include <tuple>
#include <string>
#include <fstream>
#define This (*this)

namespace GrIso
{
    
    class GraphVertex : public ShortHashList   
    {
		public:
			explicit GraphVertex(const GraphVertex&) = default;
			GraphVertex& operator=(GraphVertex&) = delete;
			GraphVertex() {}
			GraphVertex(GraphVertex&& gv)
				: ShortHashList(std::move(gv))
			{

			}
	};

    class Graph: public std::vector<GraphVertex>    
    {
        static void Abort(const char* message) { throw std::exception(message); }
        const int None = ushort(-1);
		
	public:
		explicit Graph(const Graph&) = default;
		Graph& operator=(Graph&) = delete;
		Graph(Graph&& g)
			:std::vector<GraphVertex>(std::move(g))
		{
		}
        Graph(int vertex_count)		
        {
			resize(vertex_count);
        }

        void Append(int some_vertex, int other_vertex)
        {
            This[some_vertex].Append(other_vertex);
            This[other_vertex].Append(some_vertex);
        }

        bool Hash()
        {
            for (uint i = 0; i < size(); ++i)
            {
                if (!This[i].Hash())
                    return false;
            }
            return true;
        }

        bool Find(int some_vertex, int other_vertex) const
        {
            return This[some_vertex].Find(other_vertex);
        }

        bool Compare(const Graph & graph)
        {
            if (size() != graph.size())
                return false;
            for (uint i = 0; i < size(); ++i)
                if (graph[i].Count() != This[i].Count())
                    return false;

            for (uint i1 = 0, c1 = size(); i1 < c1; ++i1)
                for (uint i2 = 0, c2 = This[i1].Count(); i2 <c2;++i2)
                    if (!graph.Find(i1, This[i1][i2]))
                        return false;

                return true;
        }
        
        std::string ToString()
        {
			auto ToString1 = [this](std::string & s, int i1, int i2)
			{
				char b[32];
				sprintf_s(b, "[%d,%d]", i1, This[i1][i2]);
				s.append(b);
			};

			std::string s;
			ToString1(s, 0, 0);
			for (uint i1 = 0, i2 = 1, c1 = size(); i1 < c1; ++i1, i2 = 0)
				for (uint c2 = This[i1].Count(); i2 < c2; ++i2)
					ToString1(s, i1, i2);
           
			return s;
        }

		std::string ToFile()
		{
			std::string s;
			for (uint i1 = 0, i2 = 1, c1 = size(); i1 < c1; ++i1, i2 = 0)
				for (uint c2 = This[i1].Count(); i2 < c2; ++i2)
				{
					char buf[32];
					sprintf_s(buf, "%d\t%d\r\n", i1, This[i1][i2]);
					s += buf;
				}
			return s;
		}

		void ToFile(const char* path)
		{
			std::ofstream stream;
			stream.open(path, std::ios_base::out | std::ios_base::binary);
			const std::string & s = ToFile();
			stream.write(s.c_str(), s.size());
			stream.close();
		}

        // Generate random connected graph.
        static Graph Generate(int vertex_count, int edge_count)
        {
            if (edge_count < vertex_count - 1)
                Abort("too less edge - only connected graph");
            if (edge_count >= vertex_count * (vertex_count - 1))
                Abort("too many edge - symetric graph");

			Graph graph(vertex_count);

            /* Explicite added and removed edges depending on bool flag. 
             * For dense graph filled only removed edges excepts those have been choosen to make connected graph. For not dense graph is simpler.
             * This gives 1/2 chance of add a random edge.
             */		
            std::set<std::tuple<int, int, bool>> edges;

            //  make graph connected
			std::vector<int> permutation;
            // list all vertex
            for (int i = 0; i < vertex_count; ++i)
                permutation.push_back(i);

            // append to connected subtree random edges
            for (int i = 1; i < vertex_count; ++i)
            {
                // vertex in subtree
                int i1 = RandQuickShared.Next(0, i - 1);
                // vertex not in subtree
                int i2 = RandQuickShared.Next(i, vertex_count - 1);
                // append new edge
                if (permutation[i1] < permutation[i2])
					edges.insert({ permutation[i1], permutation[i2], true });
                else
					edges.insert({ permutation[i2], permutation[i1], true });

                // swap selected vertex to keep booking already selected vertex in connected subtree
                i1 = permutation[i];
                permutation[i] = permutation[i2];
                permutation[i2] = i1;
            }

            if (edge_count > vertex_count * (vertex_count - 1) / 4)
            {
                for (int i = vertex_count - 1; i < vertex_count * (vertex_count - 1) / 2 - edge_count; ++i)
                {
                    int i1 = RandQuickShared.Next(0, vertex_count - 1);
                    int i2 = RandQuickShared.Next(0, vertex_count - 1);
                    if (i2 < i1) { int i3 = i1; i1 = i2; i2 = i3; }
					if (i1 != i2 && !edges.count({ i1, i2, true }) && !edges.count({ i1, i2, false }))
						edges.insert({ i1, i2, false });
                }
                for (int i1 = 0; i1 < vertex_count; ++i1)
                    for (int i2 = i1 + 1; i2 < vertex_count; ++i2)
						if (edges.count({ i1, i2, true }) || !edges.count({ i1, i2, false }))
                            graph.Append((ushort)i1, (ushort)i2);
            }
            else
            {
                for (int i = vertex_count - 1; i < edge_count; ++i)
                {
                    int i1 = RandQuickShared.Next(0, vertex_count - 1);
                    int i2 = RandQuickShared.Next(0, vertex_count - 1);
                    if (i2 < i1) { int i3 = i1; i1 = i2; i2 = i3; }
					if (i1 != i2 && !edges.count({ i1, i2, true }))
						edges.insert({ i1, i2, true });
                }

				for (auto it = edges.begin(); it != edges.end(); ++it)
					graph.Append(std::get<0>(*it), std::get<1>(*it));
            }

            if (!graph.Hash())
                Abort("Cannot make short hash");
			return graph;
        }

        static std::vector<int> Permutate(int vertex_count)
        {
			std::vector<int> permutation;
			permutation.reserve(vertex_count);
            for (int i = 0; i < vertex_count; ++i)
                permutation.push_back(i);
            for (int repeat = 2 * vertex_count; repeat > 0; --repeat)
            {
                int i1 = RandQuickShared.Next(0, vertex_count - 1);
                int i2 = RandQuickShared.Next(0, vertex_count - 1);
                int i3 = permutation[i1];
                permutation[i1] = permutation[i2];
                permutation[i2] = i3;
            }

            return permutation;
        }

        bool Compare(const Graph & graph, const std::vector<int> & permutation) const
        {
            if (size() != graph.size())
                return false;
            for (uint i = 0; i < size(); ++i)
                if (This[i].Count() != graph[permutation[i]].Count())
                    return false;

            for (uint i1 = 0,  c1 = size(); i1 < c1; ++i1)
            {
                auto & v = This[i1];
                for (uint i2 = 0, c2 = v.Count(); i2 <c2; i2 = ++i2)
                {
                    if (!graph.Find(permutation[i1], permutation[v[i2]]))
                        return false;
                }
            }

            return true;
        }

        Graph Permutate(const std::vector<int> & permutation)
        {
            Graph graph(size());
            for (uint i1 = 0; i1 < size(); ++i1)
            {
                auto & v = This[i1];
                for (uint i2 = 0, c2 = v.Count(); i2 < c2; i2 = ++i2)
                {
                    if (i1 < v[i2])
                        graph.Append(permutation[i1], permutation[v[i2]]);
                }
            }

            if (!graph.Hash())
                Abort("Cannot make short hash");
            return graph;
        }
    };
}
#pragma once
#include "memory"
#include "GraphDef.h"

namespace GrIso
{
	
    class GraphIso: public std::vector<int>
    {
	public:
		GraphIso(const Graph & some_graph_, const Graph & other_graph_)
			: some_graph(some_graph_), other_graph(other_graph_)
		{
			TryIso();
		}

	private:
		static void Abort(const char* message) { throw std::exception(message); }
		const int None = ushort(-1);

        struct VertexData
        {
            int other_vertex;
            int some_vertex;
            int border_edges;
		};

        struct EdgeData
        {
            int inside_vertex;
            int outside_vertex;
            int matched_index;
		};

        std::unique_ptr<VertexData[]> vertex_array;
		std::unique_ptr<EdgeData[]> edge_stack;
        uint vertex_cnt;
        uint edge_top;

		const Graph & some_graph;
		const Graph & other_graph;

		void TryIso()
        {
			if (some_graph.size() != other_graph.size())
				return;
            
			vertex_cnt = some_graph.size();
            vertex_array = std::unique_ptr<VertexData[]>(new VertexData[vertex_cnt]);
            edge_stack = std::unique_ptr<EdgeData[]>(new EdgeData[vertex_cnt]);

            uint some_vertex = None, other_vertex = None, border_edges = 0;
            for (uint i = 0; i < vertex_cnt; ++i)
                if (some_graph[i].Count() > border_edges)
                {
                    some_vertex = i;
                    border_edges = some_graph[i].Count();
                }
            for (uint i = 0; i < vertex_cnt; ++i)
                if (other_graph[i].Count() == border_edges)
                {
                    other_vertex = i;
                    if (TryIso(some_vertex, other_vertex))
                        GetIso();
                }
        }

		void GetIso()
        {
			reserve(vertex_cnt);
            for (uint i = 0; i < vertex_cnt; ++i)
                push_back(vertex_array[i].other_vertex);
            if (!some_graph.Compare(other_graph, (std::vector<int>&)*this))
                Abort("This lies that it has found.");
        }

        // Select next outside vertex for partial graph
        bool NextSomeVertex()
        {
            if (edge_top == vertex_cnt - 1)
                return false;
            ++edge_top;
            if (edge_stack[edge_top].outside_vertex != None)
                return true;

            int outside_vertex = -1;
            int inside_vertex = -1;
            int border_edges = 0;

            // select ouside vertex basing on heuristic that is good to have many connection to partial graph: more chance to find error in matching
            for (uint vertex = 0; vertex < vertex_cnt; ++vertex)
                if (vertex_array[vertex].other_vertex == None && vertex_array[vertex].border_edges > border_edges)
                {
                    border_edges = vertex_array[vertex].border_edges;
                    outside_vertex = vertex;
                }

            // select inside vertex basing on heuristic that is good to have not many connection beyond partial graph
            border_edges = vertex_cnt;
            for (uint vertex_index = 0, vertext_count = some_graph[outside_vertex].Count(); vertex_index<vertext_count;++vertex_index)
            {
                uint vertex = some_graph[outside_vertex][vertex_index];
                if (vertex_array[vertex].other_vertex != None && vertex_array[vertex].border_edges < border_edges)
                {
                    inside_vertex = vertex;
                    border_edges = vertex_array[vertex].border_edges;
                }
            }

            // update border edges due to moving outside vertex to partial graph
            for (int vertex_index = 0, vertext_count = some_graph[outside_vertex].Count(); vertex_index < vertext_count; ++vertex_index)
            {
                    auto vertex = some_graph[outside_vertex][vertex_index];
                    vertex_array[vertex].border_edges += vertex_array[vertex].other_vertex == None ? 1 : -1;
            }

            // push edge attaching outside vertex to partial graph
            edge_stack[edge_top].outside_vertex = outside_vertex;
            edge_stack[edge_top].inside_vertex = inside_vertex;
            edge_stack[edge_top].matched_index = -1;
            return true;
        }

        // remove vertex from partial graph and edge from stack
        bool PrevSomeVertex()
        {
            if (--edge_top == 0)
                return false;

            return true;
        }

        bool TryIso(int some_vertex, int other_vertex)
        {
            // initiate vertex array
            for (uint i=0;i<vertex_cnt;++i)
            {
                vertex_array[i].other_vertex = None;
                vertex_array[i].some_vertex = None;
                vertex_array[i].border_edges = some_graph.Find(some_vertex, i) ? 1 : 0;
            }
            vertex_array[some_vertex].other_vertex = other_vertex;
            vertex_array[other_vertex].some_vertex = some_vertex;
            vertex_array[some_vertex].border_edges = some_graph[some_vertex].Count();

            // initiate edge stack
            edge_top = 0;
            edge_stack[0].inside_vertex = None;
            edge_stack[0].matched_index= -1;
            edge_stack[0].outside_vertex = some_vertex;
            
            for (uint i = 1; i < vertex_cnt; ++i)
            {
                edge_stack[i].inside_vertex = None;
                edge_stack[i].matched_index = -1;
                edge_stack[i].outside_vertex = None;
            }

            // try build iso
            while (true)
            {
                if (!NextSomeVertex())
                    return true;

                while (true)
                {
                    if (NextOtherVertex())
                        break;
                    if (!PrevSomeVertex())
                        return false;
                }
            }
        }

        // Seek in other graph potential match for outside vertex
        bool NextOtherVertex()
        {
            int inside_vertex = edge_stack[edge_top].inside_vertex;
            int outside_vertex = edge_stack[edge_top].outside_vertex;
            vertex_array[outside_vertex].other_vertex = None;
            
            auto & other_vertex = other_graph[vertex_array[inside_vertex].other_vertex];
            int matched_count = other_vertex.Count();
            int matched_index = edge_stack[edge_top].matched_index;

            if (matched_index != -1)
            {
                vertex_array[other_vertex[matched_index]].some_vertex = None;
                edge_stack[edge_top].matched_index = -1;
            }

            while (true)
            {
                // Next potential match
                if (++matched_index == matched_count)
                    return false;
                int matched_vertex = other_vertex [matched_index];
                
                // check whether not matched yet 
                if (vertex_array[matched_vertex].some_vertex != None)
                    continue;

                // check count of neighbours
                if (other_graph[matched_vertex].Count() != some_graph[outside_vertex].Count())
                    continue;

                // check compatibilty of potential match with current inside partial graph
                int inside_count = 0;
                for (int vertex_index = 0, vertext_count = some_graph[outside_vertex].Count(); inside_count!=-1 && vertex_index < vertext_count; ++vertex_index)
                {
                    auto vertex = some_graph[outside_vertex][vertex_index];
                    if (vertex_array[vertex].other_vertex != None)
                    {
                        if (!other_graph.Find(vertex_array[vertex].other_vertex, matched_vertex))
                            inside_count = -1;
                        else
                            ++inside_count;
                    }
                }
                if (inside_count == -1)
                    continue;

                for (int vertex_index = 0, vertext_count = other_graph[matched_vertex].Count(); inside_count != -1 && vertex_index < vertext_count; ++vertex_index)
                {
                    auto vertex = other_graph[matched_vertex][vertex_index];
                    if (vertex_array[vertex].some_vertex != None)
                        --inside_count;
                }
                if (inside_count != 0)
                    continue;

                // found
                edge_stack[edge_top].matched_index = matched_index;
                vertex_array[outside_vertex].other_vertex = matched_vertex;
                vertex_array[matched_vertex].some_vertex = outside_vertex;
                return true;
            }
        }
	};
}
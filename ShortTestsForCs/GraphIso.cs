using System.Collections.Generic;

namespace GrIso
{
    class GraphIso
    {
        const int None = GraphVertex.None;
        struct VertexData
        {
            public int match_vertex;
            public int border_edges;
        }
        struct EdgeData
        {
            public int inside_vertex;
            public int outside_vertex;
            public int matched_vertex;
        }
        VertexData[] vertex_array;
        EdgeData[] edge_stack;
        int vertex_len;
        int edge_top;
        Graph some_graph;
        Graph other_graph;

        public List<int>TryIso(Graph some_graph, Graph other_graph)
        {
            if (some_graph.Count != other_graph.Count)
                return null;
            for (int i = 0;i < some_graph.Count;++i)
            if (some_graph[i].Count() != other_graph[i].Count())
                return null;

            this.some_graph = some_graph;
            this.other_graph = other_graph;
            vertex_array = new VertexData[some_graph.Count];
            vertex_len = some_graph.Count;
            edge_stack = new EdgeData[some_graph.Count];

            int some_vertex = None, other_vertex = None, border_edges = 0;
            for (int i = 0; i < some_graph.Count; ++i)
                if (some_graph[i].Count() > border_edges)
                {
                    some_vertex = i;
                    border_edges = some_graph[i].Count();
                }
            for (int i = 0; i < other_graph.Count; ++i)
                if (other_graph[i].Count() == border_edges)
                {
                    other_vertex = i;
                    if (TryIso(some_vertex, other_vertex))
                        return GetIso();
                }

            return null;
        }

        List<int> GetIso()
        {
            var permutation = new List<int>(vertex_len);
            for (int i = 0; i < vertex_len; ++i)
                permutation.Add(vertex_array[i].match_vertex);
            if (some_graph.Compare(other_graph, permutation))
                throw new AbortException("This lies that it has found.");
            return permutation;
        }

        // Select next outside vertex for partial graph
        bool NextSomeVertex()
        {
            int outside_vertex = -1;
            int inside_vertex = -1;
            int border_edges = 0;

            // select ouside vertex basing on heuristic that is good to have many connection to partial graph: more chance to find error in matching
            for (int vertex = 0; vertex < vertex_len; ++vertex)
                if (vertex_array[vertex].match_vertex == None && vertex_array[vertex].border_edges > border_edges)
                    outside_vertex = vertex;

            // select inside vertex basing on heuristic that is good to have not many connection beyond partial graph
            border_edges = some_graph.Count;
            for (int vertex = some_graph[inside_vertex].First(); vertex != None; vertex = some_graph[inside_vertex].Next(vertex))
                if (vertex_array[vertex].border_edges < border_edges)
                    inside_vertex = vertex;

            // update border edges due to moving outside vertex to partial graph
            for (int vertex = some_graph[outside_vertex].First(); vertex != None; vertex = some_graph[outside_vertex].Next(vertex))
                vertex_array[vertex].border_edges += vertex_array[vertex].match_vertex == None ? 1 : -1;

            // push edge attaching outside vertex to partial graph
            ++edge_top;
            edge_stack[edge_top].outside_vertex = outside_vertex;
            edge_stack[edge_top].inside_vertex = inside_vertex;
            edge_stack[edge_top].matched_vertex = None;

            return true;
        }

        // remove vertex from partial graph and edge from stack
        bool DropSomeVertex()
        {
            int outside_vertex = edge_stack[edge_top].outside_vertex;
            if (--edge_top == 0)
                return false;
            
            // update border edges due to removing inside vertex from partial graph
            for (int vertex = some_graph[outside_vertex].First(); vertex != None; vertex = some_graph[outside_vertex].Next(vertex))
                vertex_array[vertex].border_edges += vertex_array[vertex].match_vertex != None ? 1 : -1;

            return true;
        }

        bool TryIso(int some_vertex, int other_vertex)
        {
            // initiate vertex array
            for (int i=0;i<vertex_len;++i)
            {
                vertex_array[i].match_vertex = None;
                vertex_array[i].border_edges = some_graph.Find(some_vertex, i) ? 1 : 0;
            }
            vertex_array[some_vertex].match_vertex = other_vertex;
            vertex_array[some_vertex].border_edges = some_graph[some_vertex].Count();

            // initiate edge stack
            edge_top = 0;
            edge_stack[0].inside_vertex = some_vertex;
            edge_stack[0].matched_vertex = other_vertex;
            edge_stack[0].outside_vertex = None;

            // try build iso
            while (true)
            {
                if (!NextSomeVertex())
                    return true;

                while (true)
                {
                    if (NextOtherVertex())
                        return true;
                    if (!DropSomeVertex())
                        return false;
                }
            }
        }

        // Seek in other graph potential match for outside vertex
        bool NextOtherVertex()
        {
            int matched_vertex = edge_stack[edge_top].matched_vertex;
            int inside_vertex = edge_stack[edge_top].inside_vertex;
            int outside_vertex = edge_stack[edge_top].outside_vertex;

            while (true)
            {

                // Next potential match
                matched_vertex = other_graph[vertex_array[inside_vertex].match_vertex].Next(matched_vertex);
                if (matched_vertex == None)
                    return false;

                // check count of neighbours
                if (other_graph[matched_vertex].Count() != some_graph[outside_vertex].Count())
                    continue;

                // check compatibilty of potential match with current inside partial graph
                for (int vertex = some_graph[outside_vertex].First(); vertex != None; vertex = some_graph[outside_vertex].Next(vertex))
                    if (vertex_array[vertex].match_vertex != None && !other_graph.Find(vertex_array[vertex].match_vertex, matched_vertex))
                        continue;

                // found
                edge_stack[edge_top].matched_vertex = matched_vertex;
                return true;
            }
        }
    }
}
using System.Collections.Generic;

namespace GrIso
{
    class GraphIso
    {
        // None vertex
        const int None = -1;

        // Vertex matching data, other_vertex is matched in other graph and some vertex is matched in some graph, as you already guess naming convention we try match some graph to other graph
        struct VertexData
        {
            public int other_vertex;
            public int some_vertex;
        }

        // Edge matching data, inside vertex is already matched, outside vertex is tried to match, enumerator keep possible 
        struct EdgeData
        {
            public int inside_vertex;
            public int outside_vertex;
            public IEnumerator<int> matched_vertex;
        }

        // Vertex equal to index in some or other graph is matched to other or some vertex
        VertexData[] vertex_array;
        // Edge matching stack
        EdgeData[] edge_stack;
        int vertex_length;
        int edge_top;
        Graph some_graph;
        Graph other_graph;

        // Calculated cost of this graph iso ( exactly numver of checked verticies)
        double graph_iso_cost;
        public double GraphIsoCost => graph_iso_cost;

        public List<int> TryIso(Graph some_graph, Graph other_graph)
        {
            if (some_graph.Count != other_graph.Count)
                return null;

            this.some_graph = some_graph;
            this.other_graph = other_graph;
            vertex_length = some_graph.Count;
            if (vertex_array == null || vertex_array.Length != vertex_length)
            {
                vertex_array = new VertexData[some_graph.Count];
                edge_stack = new EdgeData[some_graph.Count];
            }

            bool found = TryIso();
            return found ? GetIso() : null;
        }

        List<int> GetIso()
        {
            var permutation = new List<int>(vertex_length);
            for (int i = 0; i < vertex_length; ++i)
                permutation.Add(vertex_array[i].other_vertex);
            return permutation;
        }

        void BuildStack()
        {
            int inside_vertex = None, outside_vertex = None, border_edges = 0;
            // find vertex with maximal count of neighbours to start searching from - we try build match in way to have a lot of constraints to easier break dead searching
            for (int vertex = 0; vertex < vertex_length; ++vertex)
                if (some_graph[vertex].Count > border_edges)
                {
                    outside_vertex = vertex;
                    border_edges = some_graph[vertex].Count;
                }

            // utilize for the moment field some_vertex to count outside vertices connected to current partial inside vertices
            for (int vertex = 0, c = vertex_array.Length; vertex < c; ++vertex)
                vertex_array[vertex].some_vertex = 0;

            // initialize stack
            edge_top = 0;
            edge_stack[0].inside_vertex = None;
            edge_stack[0].outside_vertex = outside_vertex;
            vertex_array[outside_vertex].some_vertex = None;

            // build search stack
            while (edge_top + 1 < vertex_length)
            {
                inside_vertex = outside_vertex; outside_vertex = None; border_edges = 0;
                // select ouside vertex basing on heuristic that is good to have many connection to partial graph: more chance to find error in matching
                for (int vertex = 0; vertex < vertex_length; ++vertex) if (vertex_array[vertex].some_vertex != None)
                    {
                        // update connection count for last selected vertex
                        if (some_graph[inside_vertex].Contains(vertex))
                            ++vertex_array[vertex].some_vertex;
                        // check border edge count
                        if (vertex_array[vertex].some_vertex > border_edges)
                        {
                            border_edges = vertex_array[vertex].some_vertex;
                            outside_vertex = vertex;
                        }
                    }
                // select inside vertex basing on heuristic that is good to have not many connection beyond partial graph
                inside_vertex = None; border_edges = vertex_length;
                foreach (int vertex in some_graph[outside_vertex])
                {
                    if (vertex_array[vertex].some_vertex == None && vertex_array[vertex].some_vertex < border_edges)
                    {
                        inside_vertex = vertex;
                        border_edges = vertex_array[vertex].some_vertex;
                    }
                }

                // push next edge to match on edge matching stack
                ++edge_top;
                edge_stack[edge_top].inside_vertex = inside_vertex;
                edge_stack[edge_top].outside_vertex = outside_vertex;
                vertex_array[outside_vertex].some_vertex = None;
            }
        }


        bool TryIso()
        {
            BuildStack();
            for ( int vertex = 0;vertex < vertex_length; ++vertex)
            {
                vertex_array[vertex].some_vertex = None;
                vertex_array[vertex].other_vertex = None;
            }

            graph_iso_cost = 0;
            edge_top = 0;
            edge_stack[0].matched_vertex = TryFirstMatch();
            EdgeData edge_data = edge_stack[0];

            for (; ;)
            {
                bool matched;
                
                while (matched = edge_data.matched_vertex.MoveNext())
                    if (TryNextMatch(edge_data.inside_vertex, edge_data.outside_vertex, edge_data.matched_vertex.Current))
                        break;

                ++graph_iso_cost;

                if (matched)
                {
                    vertex_array[edge_data.outside_vertex].other_vertex = edge_data.matched_vertex.Current;
                    vertex_array[edge_data.matched_vertex.Current].some_vertex = edge_data.outside_vertex;
                    if (++edge_top >= vertex_length)
                        return true;
                    edge_data = edge_stack[edge_top];
                    edge_data.matched_vertex = other_graph[vertex_array[edge_data.inside_vertex].other_vertex].GetEnumerator();
                    edge_stack[edge_top].matched_vertex = edge_data.matched_vertex;
                }
                else
                {
                    if (--edge_top < 0)
                        return false;
                    edge_data = edge_stack[edge_top];
                    vertex_array[edge_data.outside_vertex].other_vertex = None;
                    vertex_array[edge_data.matched_vertex.Current].some_vertex = None;
                }
            }
        }

        IEnumerator<int> TryFirstMatch()
        {
            for (int vertex = 0; vertex < vertex_length; ++vertex)
                if (other_graph[vertex].Count == some_graph[edge_stack[0].outside_vertex].Count)
                    yield return vertex;
        }

        bool TryNextMatch(int inside_vertex, int outside_vertex, int matched_vertex)
        {
            // check whether not matched yet 
            if (vertex_array[matched_vertex].some_vertex != None)
                return false;

            // check count of neighbours
            if (other_graph[matched_vertex].Count != some_graph[outside_vertex].Count)
                return false;

            // check compatibilty of potential match with current inside partial graph
            int inside_count = 0;
            foreach (int vertex in some_graph[outside_vertex])
            {
                if (vertex_array[vertex].other_vertex != None)
                {
                    if (!other_graph.Find(vertex_array[vertex].other_vertex, matched_vertex))
                        return false;
                    else
                        ++inside_count;
                }
            }

            foreach (int vertex in other_graph[matched_vertex])
            {
                if (vertex_array[vertex].some_vertex != None)
                    --inside_count;
            }
            if (inside_count != 0)
                return false;

            return true;
        }
    }
}
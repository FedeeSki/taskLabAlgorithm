#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <numeric> // For std::accumulate

class CoolGraph 
{
private:
    int v_count; // Renamed from num_vertices
    std::vector<std::list<int>> adj; // Renamed from adj_list
    bool directed_graph; // Renamed from is_directed

public:
    CoolGraph(int vertices, bool directed = false) 
    {
        v_count = vertices;
        adj.resize(vertices);
        directed_graph = directed;
    }

    void add_edge(int source, int destination) 
    {
        if (source >= v_count || destination >= v_count) {
            std::cerr << "Error: Vertex index is out of bounds." << std::endl;
            return;
        }
        adj[source].push_back(destination);
        if (!directed_graph) {
            adj[destination].push_back(source); // A small logic change for clarity
        }
    }

    int count_edges() {
        int total_edges = 0;
        for(int i = 0; i < v_count; i++) {
            total_edges += adj[i].size();
        }
        if (!directed_graph) {
            return total_edges / 2; // Each edge is counted twice
        }
        return total_edges;
    }

    void bfs(int start_node) 
    {
        if (start_node >= v_count) {
            std::cerr << "Error: Start node for BFS is out of bounds." << std::endl;
            return;
        }

        std::vector<bool> visited_nodes(v_count, false);
        std::queue<int> processing_queue;

        visited_nodes[start_node] = true;
        processing_queue.push(start_node);

        std::cout << "BFS Traversal starting from node " << start_node << ":" << std::endl;

        while (!processing_queue.empty()) {
            int current = processing_queue.front();
            std::cout << current << " ";
            processing_queue.pop();

            for (int neighbor_node : adj[current]) {
                if (!visited_nodes[neighbor_node]) {
                    visited_nodes[neighbor_node] = true;
                    processing_queue.push(neighbor_node);
                }
            }
        }
        std::cout << std::endl;
    }

    void dfs_recursive_util(int v, std::vector<bool>& visited) 
    {
        visited[v] = true;
        std::cout << v << " ";

        for (int neighbor : adj[v]) {
            if (!visited[neighbor]) {
                dfs_recursive_util(neighbor, visited);
            }
        }
    }

    void dfs(int start_node) 
    {
        if (start_node >= v_count) {
            std::cerr << "Error: Start node for DFS is out of bounds." << std::endl;
            return;
        }
        std::vector<bool> visited(v_count, false);
        std::cout << "DFS Traversal starting from node " << start_node << ":" << std::endl;
        dfs_recursive_util(start_node, visited);
        std::cout << std::endl;
    }
};

int main() 
{
    std::cout << "--- CoolGraph Data Structure Test ---" << std::endl;

    CoolGraph my_cool_graph(10);

    my_cool_graph.add_edge(0, 1);
    my_cool_graph.add_edge(0, 3); 
    my_cool_graph.add_edge(1, 3);
    my_cool_graph.add_edge(1, 4);
    my_cool_graph.add_edge(2, 5);
    my_cool_graph.add_edge(2, 6);
    my_cool_graph.add_edge(3, 8); 
    my_cool_graph.add_edge(4, 8); 
    my_cool_graph.add_edge(5, 9); 
    my_cool_graph.add_edge(6, 9); 
    my_cool_graph.add_edge(8, 9); 
    
    std::cout << "Total edges: " << my_cool_graph.count_edges() << std::endl;
    std::cout << std::endl;

    my_cool_graph.bfs(1);

    my_cool_graph.dfs(1);

    std::cout << "\n--- Test Has Been Completed ---" << std::endl;

    return 0;
}

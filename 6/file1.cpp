#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>

class Graph {
private:
    int num_vertices;
    std::vector<std::list<int>> adj_list;
    bool is_directed;

public:
    Graph(int vertices, bool directed = false) {
        num_vertices = vertices;
        adj_list.resize(vertices);
        is_directed = directed;
    }

    void add_edge(int src, int dest) {
        if (src >= num_vertices || dest >= num_vertices) {
            std::cerr << "Error: Vertex index out of bounds." << std::endl;
            return;
        }
        adj_list[src].push_back(dest);
        if (!is_directed) {
            adj_list[dest].push_back(src);
        }
    }

    void print_graph() {
        std::cout << "Graph Adjacency List:" << std::endl;
        for (int i = 0; i < num_vertices; ++i) {
            std::cout << "Vertex " << i << ":";
            for (int neighbor : adj_list[i]) {
                std::cout << " -> " << neighbor;
            }
            std::cout << std::endl;
        }
    }

    void bfs(int start_vertex) {
        if (start_vertex >= num_vertices) {
            std::cerr << "Error: Start vertex for BFS is out of bounds." << std::endl;
            return;
        }

        std::vector<bool> visited(num_vertices, false);
        std::queue<int> q;

        visited[start_vertex] = true;
        q.push(start_vertex);

        std::cout << "BFS Traversal starting from vertex " << start_vertex << ":" << std::endl;

        while (!q.empty()) {
            int current_vertex = q.front();
            std::cout << current_vertex << " ";
            q.pop();

            for (int neighbor : adj_list[current_vertex]) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        std::cout << std::endl;
    }

    void dfs_util(int vertex, std::vector<bool>& visited) {
        visited[vertex] = true;
        std::cout << vertex << " ";

        for (int neighbor : adj_list[vertex]) {
            if (!visited[neighbor]) {
                dfs_util(neighbor, visited);
            }
        }
    }

    void dfs(int start_vertex) {
        if (start_vertex >= num_vertices) {
            std::cerr << "Error: Start vertex for DFS is out of bounds." << std::endl;
            return;
        }
        std::vector<bool> visited(num_vertices, false);
        std::cout << "DFS Traversal starting from vertex " << start_vertex << ":" << std::endl;
        dfs_util(start_vertex, visited);
        std::cout << std::endl;
    }
};

int main() {
    std::cout << "--- Graph Data Structure Test ---" << std::endl;

    Graph my_graph(8);

    my_graph.add_edge(0, 1);
    my_graph.add_edge(0, 2);
    my_graph.add_edge(1, 3);
    my_graph.add_edge(1, 4);
    my_graph.add_edge(2, 5);
    my_graph.add_edge(2, 6);
    my_graph.add_edge(3, 7);
    my_graph.add_edge(4, 7);
    my_graph.add_edge(5, 7);
    my_graph.add_edge(6, 7);
    
    my_graph.print_graph();
    std::cout << std::endl;

    my_graph.bfs(0);

    my_graph.dfs(0);

    std::cout << "\n--- Test Complete ---" << std::endl;

    return 0;
}

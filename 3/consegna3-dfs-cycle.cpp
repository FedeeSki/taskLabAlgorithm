#include <iostream>

// Struttura per un nodo della lista di adiacenza
struct AdjListNode {
    int dest;
    AdjListNode* next;
};

// Struttura per rappresentare il grafo
struct Graph {
    int V; // Numero di vertici
    AdjListNode** adj; // Array di puntatori alle liste di adiacenza
};

//////////////// FUNZIONI PER GRAFO ////////////////////////////////////////

// Funzione per creare un nuovo nodo della lista di adiacenza
AdjListNode* newAdjListNode(int dest) {
    AdjListNode* newNode = new AdjListNode();
    newNode->dest = dest;
    newNode->next = nullptr;
    return newNode;
}

// Funzione per creare un grafo con V vertici
Graph* createGraph(int V) {
    Graph* graph = new Graph();
    graph->V = V;
    graph->adj = new AdjListNode*[V];
    for (int i = 0; i < V; ++i) {
        graph->adj[i] = nullptr;
    }
    return graph;
}

// Funzione per aggiungere un arco orientato da src a dest
void addEdge(Graph* graph, int src, int dest) {
    // Aggiunge il nodo all'inizio della lista di src
    AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->adj[src];
    graph->adj[src] = newNode;
}

// Funzione ausiliaria ricorsiva per la DFS
// visited: 0 = non visitato, 1 = in corso di visita (nello stack di ricorsione), 2 = visitato

// // Funzione principale per rilevare cicli in un grafo
// bool isCyclic(Graph* graph) {
//     if (graph == nullptr) return false;

//     // Array per tenere traccia dei nodi visitati
//     // 0 = non visitato, 1 = in corso, 2 = visitato
//     int* visited = new int[graph->V];
//     for (int i = 0; i < graph->V; ++i) {
//         visited[i] = 0;
//     }

//     // Esegue la DFS per ogni vertice non ancora visitato
//     // (necessario per grafi non connessi)
//     for (int i = 0; i < graph->V; ++i) {
//         if (visited[i] == 0) {
//             if (isCyclicUtil(i, graph, visited)) {
//                 delete[] visited;
//                 return true; 
//             }
//         }
//     }

//     delete[] visited;
//     return false; 
// }

//////////////// FUNZIONI PER GRAFO ////////////////////////////////////////

// Sub-routine ricorsiva per la DFS che trova il ciclo più lungo
void findLongestCycleUtil(int u, Graph* graph, int* visited, int* path, int& path_len, int& max_cycle_len, int*& longest_cycle_nodes) {
    visited[u] = 1; // Marca il nodo come "in corso di visita"
    path[path_len++] = u; // Aggiunge il nodo al percorso corrente

    AdjListNode* currentNode = graph->adj[u];
    while (currentNode != nullptr) {
        int v = currentNode->dest;

        if (visited[v] == 1) { // Trovato un arco all'indietro, quindi un ciclo
            int current_cycle_len = 0;
            int cycle_start_index = -1;
            // Trova l'inizio del ciclo nel percorso corrente
            for (int i = 0; i < path_len; ++i) {
                if (path[i] == v) {
                    cycle_start_index = i; //trovo dove v appare
                    break;
                }
            }

            if (cycle_start_index != -1) {
                current_cycle_len = path_len - cycle_start_index; //calcolo lunghezza

                if (current_cycle_len > max_cycle_len) {
                    max_cycle_len = current_cycle_len;
                    // Salva i nodi del nuovo ciclo più lungo
                    delete[] longest_cycle_nodes; // Libera la memoria del ciclo precedente
                    longest_cycle_nodes = new int[max_cycle_len];
                    for (int i = 0; i < max_cycle_len; ++i) {
                        longest_cycle_nodes[i] = path[cycle_start_index + i]; //salvo il path del ciclo più lungo
                    }
                }
            }
        } else if (visited[v] == 0) {
            findLongestCycleUtil(v, graph, visited, path, path_len, max_cycle_len, longest_cycle_nodes);
        }
        currentNode = currentNode->next;
    }

    visited[u] = 2; // Marca il nodo come "completamente visitato"
    path_len--; // Rimuove il nodo dal percorso corrente
}

// Funzione principale per trovare il ciclo più lungo in un grafo
int findLongestCycle(Graph* graph, int*& longest_cycle_nodes) {
    if (graph == nullptr) return 0;

    int* visited = new int[graph->V];
    int* path = new int[graph->V];
    int path_len = 0;
    int max_cycle_len = 0;
    longest_cycle_nodes = nullptr;

    for (int i = 0; i < graph->V; ++i) {
        visited[i] = 0;
    }

    // La visita parte da ogni nodo per coprire anche grafi non connessi
    for (int i = 0; i < graph->V; ++i) {
        if (visited[i] == 0) {
            findLongestCycleUtil(i, graph, visited, path, path_len, max_cycle_len, longest_cycle_nodes);
        }
    }

    delete[] visited;
    delete[] path;
    return max_cycle_len;
}


// Funzione per deallocare la memoria del grafo
void destroyGraph(Graph* graph) {
    if (graph == nullptr) return;
    for (int i = 0; i < graph->V; ++i) {
        AdjListNode* currentNode = graph->adj[i];
        while (currentNode != nullptr) {
            AdjListNode* temp = currentNode;
            currentNode = currentNode->next;
            delete temp;
        }
    }
    delete[] graph->adj;
    delete graph;
}

// Funzione main per testare il codice
int main() {
    // Esempio con grafo disconnesso e cicli multipli
    std::cout << "--- Test con un grafo disconnesso ---" << std::endl;
    int V = 9;
    Graph* graph = createGraph(V);

    // Componente 1: Ciclo di lunghezza 3
    addEdge(graph, 0, 1);
    addEdge(graph, 1, 2);
    addEdge(graph, 2, 0);

    // Componente 2: Ciclo di lunghezza 4
    addEdge(graph, 3, 4);
    addEdge(graph, 4, 5);
    addEdge(graph, 5, 6);
    addEdge(graph, 6, 3);

    // Componente 3: Un percorso aciclico
    addEdge(graph, 7, 8);

    int* longest_cycle_nodes = nullptr;
    int longest_cycle_len = findLongestCycle(graph, longest_cycle_nodes);

    if (longest_cycle_len > 0) {
        std::cout << "Il grafo contiene cicli." << std::endl;
        std::cout << "La lunghezza del ciclo piu' lungo e': " << longest_cycle_len << std::endl;
        std::cout << "Nodi del ciclo piu' lungo: ";
        for (int i = 0; i < longest_cycle_len; ++i) {
            std::cout << longest_cycle_nodes[i] << (i == longest_cycle_len - 1 ? "" : " -> ");
        }
        std::cout << std::endl;
    } else {
        std::cout << "Il grafo non contiene cicli." << std::endl;
    }

    delete[] longest_cycle_nodes;
    destroyGraph(graph);

    return 0;
}



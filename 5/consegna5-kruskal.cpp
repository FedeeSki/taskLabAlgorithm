#include <iostream>

// Struttura per rappresentare un arco pesato nel grafo
struct Edge {
    int src, dest, weight;
};

// Struttura per rappresentare il grafo
// Simile alla struttura del task 3, ma utilizza un array di archi
// invece di una lista di adiacenza, poiché è più adatto per l'algoritmo di Kruskal.
struct Graph {
    int V; // Numero di vertici
    int E; // Numero di archi (capacità massima)
    int edge_count; // Numero di archi attualmente presenti
    Edge* edges; // Array di tutti gli archi
};

// Struttura per i sottoinsiemi utilizzati nella DSU (Disjoint Set Union)
// Questa struttura è essenziale per rilevare i cicli.
struct Subset {
    int parent;
    int rank; // Il "rank" aiuta a mantenere l'albero piatto durante l'unione
};

// Funzione per creare un grafo con V vertici e E archi
Graph* createGraph(int V, int E) {
    Graph* graph = new Graph();
    graph->V = V;
    graph->E = E;
    graph->edge_count = 0;
    graph->edges = new Edge[E]; // Alloca memoria per E archi
    return graph;
}

// Funzione per aggiungere un arco al grafo
void addEdge(Graph* graph, int src, int dest, int weight) {
    // Aggiunge l'arco solo se c'è spazio
    if (graph->edge_count < graph->E) {
        graph->edges[graph->edge_count].src = src;
        graph->edges[graph->edge_count].dest = dest;
        graph->edges[graph->edge_count].weight = weight;
        graph->edge_count++;
    }
}

//////////////// FUNZIONI PER DISJOINT SET UNION (DSU) ///////////////////

// Trova il rappresentante dell'insieme a cui appartiene 'i' (con path compression)
// La path compression ottimizza le ricerche future rendendo l'albero più piatto.
int find(Subset subsets[], int i) {
    if (subsets[i].parent != i) {
        // Se 'i' non è il genitore di se stesso, allora non è il rappresentante.
        // Si risale l'albero e si imposta il genitore di 'i' direttamente al rappresentante.
        subsets[i].parent = find(subsets, subsets[i].parent);
    }
    return subsets[i].parent;
}

// Unisce due insiemi x e y (union by rank)
// L'union by rank assicura che l'albero più piccolo sia attaccato alla radice
// dell'albero più grande, mantenendo la profondità bassa.
void Union(Subset subsets[], int x, int y) {
    int xroot = find(subsets, x);
    int yroot = find(subsets, y);

    // Attacca l'albero con rank inferiore sotto la radice dell'albero con rank superiore
    if (subsets[xroot].rank < subsets[yroot].rank) {
        subsets[xroot].parent = yroot;
    } else if (subsets[xroot].rank > subsets[yroot].rank) {
        subsets[yroot].parent = xroot;
    } else {
        // Se i rank sono uguali, ne scegliamo uno come radice e incrementiamo il suo rank
        subsets[yroot].parent = xroot;
        subsets[xroot].rank++;
    }
}

//////////////// ALGORITMO DI KRUSKAL //////////////////////////////////////

// Funzione per scambiare due archi (necessaria per QuickSort)
void swapEdges(Edge* a, Edge* b) {
    Edge t = *a;
    *a = *b;
    *b = t;
}

// Funzione di partizione per l'algoritmo QuickSort.
// Prende l'ultimo elemento come pivot e lo posiziona nella sua posizione corretta.
int partition(Edge arr[], int low, int high) {
    int pivot_weight = arr[high].weight; // Pivot
    int i = (low - 1); // Indice dell'elemento più piccolo

    for (int j = low; j <= high - 1; j++) {
        // Se l'elemento corrente ha un peso minore o uguale al pivot
        if (arr[j].weight <= pivot_weight) {
            i++;
            swapEdges(&arr[i], &arr[j]);
        }
    }
    swapEdges(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Funzione principale che implementa QuickSort
void quickSort(Edge arr[], int low, int high) {
    if (low < high) {
        // pi è l'indice di partizionamento
        int pi = partition(arr, low, high);

        // Ordina ricorsivamente gli elementi prima e dopo la partizione
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Funzione per stampare i cluster (componenti connesse)
void printClusters(Subset subsets[], int V) {
    // Questo approccio è O(V^2) ma non richiede strutture dati complesse.
    // Itera su ogni potenziale radice.
    for (int i = 0; i < V; ++i) {
        // Se 'i' è il genitore di se stesso, allora è il rappresentante di un cluster.
        if (subsets[i].parent == i) {
            bool first = true;
            std::cout << "  Cluster (radice " << i << "): { ";
            // Trova tutti i nodi che appartengono a questo cluster.
            for (int j = 0; j < V; ++j) {
                if (find(subsets, j) == i) {
                    if (!first) {
                        std::cout << ", ";
                    }
                    std::cout << j;
                    first = false;
                }
            }
            std::cout << " }" << std::endl;
        }
    }
}

// Funzione principale che implementa l'algoritmo di Kruskal
void kruskalMST(Graph* graph) {
    int V = graph->V;
    Edge* result = new Edge[V]; // L'MST avrà al massimo V-1 archi
    int e = 0; // Indice per gli archi del risultato
    int i = 0; // Indice per gli archi ordinati
    int edges_to_show[] = {3, 6, 9}; // Mostra i cluster dopo aver aggiunto 3, 6 e 9 archi
    int show_index = 0;

    // 1. Ordina tutti gli archi in ordine non decrescente di peso
    quickSort(graph->edges, 0, graph->edge_count - 1);

    // 2. Alloca memoria per i sottoinsiemi della DSU
    Subset* subsets = new Subset[V];
    for (int v = 0; v < V; ++v) {
        subsets[v].parent = v; // Ogni vertice è inizialmente genitore di se stesso
        subsets[v].rank = 0;
    }

    // L'MST avrà V-1 archi
    while (e < V - 1 && i < graph->edge_count) {
        // 3. Prendi l'arco più piccolo e incrementa l'indice per la prossima iterazione
        Edge next_edge = graph->edges[i++];

        int x = find(subsets, next_edge.src);
        int y = find(subsets, next_edge.dest);

        // Se l'inclusione di questo arco non causa un ciclo
        if (x != y) {
            result[e++] = next_edge;
            Union(subsets, x, y);

            // Mostra la divisione gerarchica ai livelli specificati
            if (show_index < sizeof(edges_to_show)/sizeof(int) && e == edges_to_show[show_index]) {
                std::cout << "\n--- Stato dei cluster dopo " << e << " archi aggiunti ---" << std::endl;
                printClusters(subsets, V);
                show_index++;
            }
        }
    }

    // Stampa il risultato finale
    std::cout << "\n--- Risultato Finale ---" << std::endl;
    std::cout << "Archi del Minimum Spanning Tree (MST):" << std::endl;
    int totalWeight = 0;
    for (i = 0; i < e; ++i) {
        std::cout << result[i].src << " -- " << result[i].dest << " (peso: " << result[i].weight << ")" << std::endl;
        totalWeight += result[i].weight;
    }
    std::cout << "Peso totale dell'MST: " << totalWeight << std::endl;

    delete[] subsets;
    delete[] result;
}

// Funzione per deallocare la memoria del grafo
void destroyGraph(Graph* graph) {
    if (graph != nullptr) {
        delete[] graph->edges; // Libera la memoria degli archi
        delete graph;
    }
}

// Funzione main per testare il codice
int main() {
    // Esempio di grafo basato sull'esempio del libro di Cormen
    int V = 9; // Numero di vertici
    int E = 14; // Numero di archi
    Graph* graph = createGraph(V, E);

    // Aggiunta degli archi: (sorgente, destinazione, peso)
    addEdge(graph, 0, 1, 4);
    addEdge(graph, 0, 7, 8);
    addEdge(graph, 1, 2, 8);
    addEdge(graph, 1, 7, 11);
    addEdge(graph, 2, 3, 7);
    addEdge(graph, 2, 8, 2);
    addEdge(graph, 2, 5, 4);
    addEdge(graph, 3, 4, 9);
    addEdge(graph, 3, 5, 14);
    addEdge(graph, 4, 5, 10);
    addEdge(graph, 5, 6, 2);
    addEdge(graph, 6, 7, 1);
    addEdge(graph, 6, 8, 6);
    addEdge(graph, 7, 8, 7);

    // Applica l'algoritmo di Kruskal
    kruskalMST(graph);

    // Libera la memoria
    destroyGraph(graph);

    return 0;
}

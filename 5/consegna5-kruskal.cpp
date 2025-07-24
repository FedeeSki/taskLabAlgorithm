#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

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
void printClusters(Subset subsets[], int V, const std::map<int, int>& node_map_inv) {
    // Questo approccio è O(V^2) ma non richiede strutture dati complesse.
    // Itera su ogni potenziale radice.
    for (int i = 0; i < V; ++i) {
        // Se 'i' è il genitore di se stesso, allora è il rappresentante di un cluster.
        if (subsets[i].parent == i) {
            bool first = true;
            std::cout << "  Cluster (radice " << node_map_inv.at(i) << "): { ";
            // Trova tutti i nodi che appartengono a questo cluster.
            for (int j = 0; j < V; ++j) {
                if (find(subsets, j) == i) {
                    if (!first) {
                        std::cout << ", ";
                    }
                    std::cout << node_map_inv.at(j);
                    first = false;
                }
            }
            std::cout << " }" << std::endl;
        }
    }
}

// Funzione per generare un file .dot per la visualizzazione con Graphviz
void generateDotFile(const char* filename, Edge result[], int e, Subset subsets[], int V, const std::map<int, int>& node_map_inv) {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        std::cerr << "Errore nell'apertura del file .dot" << std::endl;
        return;
    }

    dotFile << "graph MST {" << std::endl;
    dotFile << "  layout=neato;" << std::endl;
    dotFile << "  overlap=false;" << std::endl;

    // Raggruppa i nodi per cluster
    for (int i = 0; i < V; i++) {
        if (subsets[i].parent == i) {
            dotFile << "  subgraph cluster_" << i << " {" << std::endl;
            dotFile << "    label=\"Cluster " << node_map_inv.at(i) << "\";" << std::endl;
            for (int j = 0; j < V; j++) {
                if (find(subsets, j) == i) {
                    dotFile << "    " << node_map_inv.at(j) << ";" << std::endl;
                }
            }
            dotFile << "  }" << std::endl;
        }
    }

    // Aggiunge gli archi dell'MST
    for (int i = 0; i < e; i++) {
        dotFile << "  " << node_map_inv.at(result[i].src) << " -- " << node_map_inv.at(result[i].dest)
                << " [label=\"" << result[i].weight << "\", len=" << result[i].weight / 100.0 << "];" << std::endl;
    }

    dotFile << "}" << std::endl;
    dotFile.close();
}


// Funzione principale che implementa l'algoritmo di Kruskal
void kruskalMST(Graph* graph, const std::map<int, int>& node_map_inv) {
    int V = graph->V;
    Edge* result = new Edge[V]; // L'MST avrà al massimo V-1 archi
    int e = 0; // Indice per gli archi del risultato
    int i = 0; // Indice per gli archi ordinati
    int edges_to_show[] = {10, 50, 100}; // Mostra i cluster dopo aver aggiunto 10, 50 e 100 archi
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
                printClusters(subsets, V, node_map_inv);
                
                // Genera file .dot
                std::string dot_filename = "kruskal_step_" + std::to_string(e) + ".dot";
                generateDotFile(dot_filename.c_str(), result, e, subsets, V, node_map_inv);
                std::cout << "File DOT generato: " << dot_filename << std::endl;

                show_index++;
            }
        }
    }

    // Stampa il risultato finale
    std::cout << "\n--- Risultato Finale ---" << std::endl;
    std::cout << "Archi del Minimum Spanning Tree (MST):" << std::endl;
    int totalWeight = 0;
    for (i = 0; i < e; ++i) {
        std::cout << node_map_inv.at(result[i].src) << " -- " << node_map_inv.at(result[i].dest) << " (peso: " << result[i].weight << ")" << std::endl;
        totalWeight += result[i].weight;
    }
    std::cout << "Peso totale dell'MST: " << totalWeight << std::endl;

    delete[] subsets;
    delete[] result;
}

// Funzione per caricare un grafo da un file di testo
Graph* loadGraphFromFile(const char* filename, std::map<int, int>& node_map, std::map<int, int>& node_map_inv) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Errore: Impossibile aprire il file " << filename << std::endl;
        return nullptr;
    }

    std::string line;
    int src, dest, weight;
    char comma;

    // Contiamo gli archi e mappiamo i nodi
    int E = 0;
    int next_node_idx = 0;
    
    // Primo passaggio per contare gli archi e mappare i nodi
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> src >> comma >> dest >> comma >> weight;
        if (node_map.find(src) == node_map.end()) {
            node_map[src] = next_node_idx;
            node_map_inv[next_node_idx] = src;
            next_node_idx++;
        }
        if (node_map.find(dest) == node_map.end()) {
            node_map[dest] = next_node_idx;
            node_map_inv[next_node_idx] = dest;
            next_node_idx++;
        }
        E++;
    }

    int V = node_map.size();
    Graph* graph = createGraph(V, E);

    // Riavvolgiamo il file e lo rileggiamo per aggiungere gli archi
    file.clear();
    file.seekg(0, std::ios::beg);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ss >> src >> comma >> dest >> comma >> weight;
        addEdge(graph, node_map[src], node_map[dest], weight);
    }

    file.close();
    std::cout << "Caricato grafo da " << filename << " con " << V << " vertici e " << E << " archi." << std::endl;
    return graph;
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
    const char* filename = "deepseek_DATASET_graphBOLOGNA.txt";
    std::map<int, int> node_map; // Mappa da ID originali a indici 0..V-1
    std::map<int, int> node_map_inv; // Mappa inversa da indici a ID originali

    Graph* graph = loadGraphFromFile(filename, node_map, node_map_inv);

    if (graph == nullptr) {
        return 1; // Errore nel caricamento del file
    }

    // Applica l'algoritmo di Kruskal
    kruskalMST(graph, node_map_inv);

    // Libera la memoria
    destroyGraph(graph);

    return 0;
}

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

// compilazione: g++ lezione17-grafi-dijkstra.cpp
//
// Obiettivo:
// 1) grafo con archi pesati
// 2) implementazione shortest path

#define INFTY 1000000

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0; // 1 per attivare dot view (-g linea comando)

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step


int ct_visit = 0; // contatore durante visita


///  ================== HEAP (min-heap) ==============================0


typedef struct {
    float dist;
    int node;
} HeapElement;

typedef struct {
    HeapElement* array;
    int* heap_index; // Mappa inversa: nodo → posizione nell'heap (per decrease_key efficiente)
    int size;
    int capacity;
} Heap;

int heap_parent(int i) {
    if (i == 0) return -1;
    return (i - 1) / 2;
}

int heap_child_L(int i, int size) {
    int idx = 2 * i + 1;
    if (idx >= size) return -1;
    return idx;
}

int heap_child_R(int i, int size) {
    int idx = 2 * i + 2;
    if (idx >= size) return -1;
    return idx;
}

int heap_is_leaf(int i, int size) {
    return (heap_child_L(i, size) == -1);
}

// Scambia due elementi mantenendo consistente la mappa heap_index
void swap_heap_elements(Heap* h, int i, int j) {
    HeapElement temp = h->array[i];
    h->array[i] = h->array[j];
    h->array[j] = temp;
    
    // Aggiorna la mappa inversa dopo lo scambio
    h->heap_index[h->array[i].node] = i;
    h->heap_index[h->array[j].node] = j;
}

Heap* heap_create(int capacity) {
    Heap* h = (Heap*)malloc(sizeof(Heap));
    h->array = (HeapElement*)malloc(sizeof(HeapElement) * capacity);
    h->heap_index = (int*)malloc(sizeof(int) * capacity);
    h->size = 0;
    h->capacity = capacity;
    
    // Inizializza tutti i nodi come "non presenti nell'heap"
    for (int i = 0; i < capacity; i++) h->heap_index[i] = -1;
    return h;
}

void heap_destroy(Heap* h) {
    free(h->array);
    free(h->heap_index);
    free(h);
}

void heap_insert(Heap* h, float dist, int node) {
    if (h->size >= h->capacity) return;
    
    // Inserisce in fondo e ripristina la proprietà del min-heap verso l'alto
    int i = h->size;
    h->size++;
    h->array[i].dist = dist;
    h->array[i].node = node;
    h->heap_index[node] = i;
    
    // Bubble-up: sale fino a trovare la posizione corretta
    while (i != 0) {
        int parent = heap_parent(i);
        if (h->array[parent].dist > h->array[i].dist) {
            swap_heap_elements(h, i, parent);
            i = parent;
        } else break;
    }
}

// Ripristina la proprietà del min-heap scendendo dall'indice i
void heapify(Heap* h, int i) {
    while (1) {
        int left = heap_child_L(i, h->size);
        int right = heap_child_R(i, h->size);
        int smallest = i;
        
        if (left != -1 && h->array[left].dist < h->array[smallest].dist)
            smallest = left;
        if (right != -1 && h->array[right].dist < h->array[smallest].dist)
            smallest = right;
            
        if (smallest == i) break;
        swap_heap_elements(h, i, smallest);
        i = smallest;
    }
}

HeapElement heap_remove_min(Heap* h) {
    if (h->size <= 0) return {INFTY, -1};
    
    HeapElement min_elem = h->array[0];
    h->heap_index[min_elem.node] = -1; // Segna come rimosso dall'heap
    
    // Sposta l'ultimo elemento in cima e ripristina l'heap
    h->array[0] = h->array[h->size - 1];
    h->heap_index[h->array[0].node] = 0;
    h->size--;
    heapify(h, 0);
    
    return min_elem;
}

// Operazione chiave per Dijkstra: diminuisce la distanza di un nodo già nell'heap
void heap_decrease_key(Heap* h, int node, float new_dist) {
    int i = h->heap_index[node];
    if (i == -1 || new_dist > h->array[i].dist) return; // Nodo non presente o chiave maggiore
    
    h->array[i].dist = new_dist;
    
    // Bubble-up: la nuova distanza minore può violare l'heap verso l'alto
    while (i != 0) {
        int parent = heap_parent(i);
        if (h->array[parent].dist > h->array[i].dist) {
            swap_heap_elements(h, i, parent);
            i = parent;
        } else break;
    }
}

//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node {
    int val; /// prossimo nodo
    float w; /// peso dell'arco
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list {
    node *head;
} list_t;

//////////////////////////////////////////////////
/// Definizione della struttura dati grafo
//////////////////////////////////////////////////

int *V;          // elenco dei nodi del grafo
int *V_visitato; // nodo visitato
int *V_prev;     // nodo precedente dalla visita
float *V_dist;   // distanza da sorgente

// list_t* E;  /// array con le liste di adiacenza per ogni nodo
list_t **E; 
int n_nodi;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati grafo
//////////////////////////////////////////////////

/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

list_t *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista allocata

int get_address(void *node) {
    return (int)((long)node - (long)global_ptr_ref);
}

void node_print(int n) {

    /// calcolo massima distanza (eccetto infinito)
    float max_d = 0;
    for (int i = 0; i < n_nodi; i++)
        if (max_d < V_dist[i] && V_dist[i] < INFTY)
            max_d = V_dist[i];

    output_graph << "node_" << n << "_" << n_operazione << endl;
    output_graph << "[ shape = oval; ";

    if (V_visitato[n] == 1)
        output_graph << "penwidth = 4; ";

    float col = V_dist[n] / max_d; /// distanza in scala 0..1
    output_graph << "fillcolor = \"0.0 0.0 " << col / 2 + 0.5 << "\"; style=filled; ";
    if (V_dist[n] < INFTY)
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: " << V_dist[n] << "\" ];\n";
    else
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: INF\" ];\n";

    node_t *elem = E[n]->head;
    while (elem != NULL) { /// disegno arco
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << elem->val << "_" << n_operazione << " [ label=\"" << elem->w << "\", len=" << elem->w / 100 * 10 << " ]\n";
        elem = elem->next;
    }

    if (V_prev[n] != -1) { // se c'e' un nodo precedente visitato -> disegno arco

        float len = 0;
        /*
        //cerco arco V_prev[n] --> V[n]
        node_t* elem=E[ V_prev[n] ]->head;
        while (elem!=NULL){
        int v=elem->val; /// arco u --> v
        if (v == V[n])
          len=elem->w;
        elem=elem->next;
          }
        */

        len = 1;
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << V_prev[n] << "_" << n_operazione << " [ color=blue, penwidth=5, len=" << len / 100 * 10 << " ]\n";
    }
}

void graph_print() {
    for (int i = 0; i < n_nodi; i++)
        node_print(i);
    n_operazione++;
}

void list_print(list_t *l) {
    printf("Stampa lista\n");

    if (l->head == NULL) {
        printf("Lista vuota\n");
    } else {
        node_t *current = l->head;

        while (current != NULL) {
            if (!details)
                printf("%d w:%f, ", current->val, current->w);
            else { /// stampa completa
                if (current->next == NULL)
                    printf("allocato in %d [Val: %d, W: %f, Next: NULL]\n",
                           get_address(current),
                           current->val,
                           current->w);
                else
                    printf("allocato in %d [Val: %d, W: %f, Next: %d]\n",
                           get_address(current),
                           current->val,
                           current->w,
                           get_address(current->next));
            }
            current = current->next;
        }
        printf("\n");
    }
}

list_t *list_new(void) {
    list_t *l = new list;
    if (details) {
        printf("Lista creata\n");
    }

    l->head = NULL; //// perche' non e' l.head ?
    if (details) {
        printf("Imposto a NULL head\n");
    }

    return l;
}

void list_insert_front(list_t *l, int elem, float w) {
    /// inserisce un elemento all'inizio della lista
    node_t *new_node = new node_t;
    new_node->next = NULL;

    new_node->val = elem;
    new_node->w = w;

    new_node->next = l->head;

    l->head = new_node;
}

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void print_array_graph(int *A, int n, string c, int a, int l, int m, int r) {
    /// prepara il disegno dell'array A ed il suo contenuto (n celle)
    /// a e' il codice del nodo e c la stringa
    /// l,m,r i tre indici della bisezione

    // return ;

    output_graph << c << a << " [label=<" << endl;

    /// tabella con contenuto array
    output_graph << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" > " << endl;
    /// indici
    output_graph << "<TR  >";
    for (int j = 0; j < n; j++) {
        output_graph << "<TD ";
        output_graph << ">" << j << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "<TR>";
    // contenuto
    for (int j = 0; j < n; j++) {
        output_graph << "<TD BORDER=\"1\"";
        if (j == m)
            output_graph << " bgcolor=\"#00a000\""; /// valore testato
        else if (j >= l && j <= r)
            output_graph << " bgcolor=\"#80ff80\""; /// range di competenza
        output_graph << ">" << A[j] << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "</TABLE> " << endl;

    output_graph << ">];" << endl;
}

// shortest_path fatto con min heap O((V + E)logV) al posto di O(V^2)
void shortest_path(int source) {
    // Inizializzazione: tutte le distanze infinite tranne la sorgente
    for (int i = 0; i < n_nodi; i++) {
        V_dist[i] = INFTY;
        V_prev[i] = -1;
        V_visitato[i] = 0;
    }
    V_dist[source] = 0;

    // Crea heap e inserisce tutti i nodi (sorgente con dist=0, altri con INFTY)
    Heap* h = heap_create(n_nodi);
    for (int i = 0; i < n_nodi; i++) {
        heap_insert(h, V_dist[i], i);
    }

    while (h->size > 0) {
        graph_print();
        
        // Estrae il nodo con distanza minima dall'heap
        HeapElement min_el = heap_remove_min(h);
        int u = min_el.node;
        V_visitato[u] = 1;

        // Rilassamento: controlla tutti i vicini di u
        node_t* elem = E[u]->head;
        while (elem != NULL) {
            int v = elem->val;
            float w = elem->w;
            if (!V_visitato[v]) {
                float alt = V_dist[u] + w;
                // Se trovato percorso più breve, aggiorna distanza e heap
                if (alt < V_dist[v]) {
                    V_dist[v] = alt;
                    V_prev[v] = u;
                    heap_decrease_key(h, v, alt); // Operazione O(log n) grazie all'indicizzazione
                }
            }
            elem = elem->next;
        }
    }
    heap_destroy(h);
    graph_print();
}

void bellman_ford(int source) {
    // Inizializzazione: stessa di Dijkstra ma senza heap (gestisce pesi negativi)
    for (int i = 0; i < n_nodi; i++) {
        V_dist[i] = INFTY;
        V_prev[i] = -1;
    }
    V_dist[source] = 0;

    // Rilassamento iterativo: ripete n-1 volte per garantire convergenza
    for (int i = 0; i < n_nodi - 1; i++) {
        for (int u = 0; u < n_nodi; u++) {
            node_t* elem = E[u]->head;
            while (elem != NULL) {
                int v = elem->val;
                float w = elem->w;
                // Rilassamento standard: se u è raggiungibile e il percorso u→v migliora v
                if (V_dist[u] < INFTY && V_dist[u] + w < V_dist[v]) {
                    V_dist[v] = V_dist[u] + w;
                    V_prev[v] = u;
                }
                elem = elem->next;
            }
        }
    }

    // Verifica cicli negativi: se ancora possibile rilassare, c'è un ciclo negativo
    for (int u = 0; u < n_nodi; u++) {
        node_t* elem = E[u]->head;
        while (elem != NULL) {
            int v = elem->val;
            float w = elem->w;
            if (V_dist[u] < INFTY && V_dist[u] + w < V_dist[v]) {
                printf("Trovato ciclo negativo!\n");

                // Reset completo: ciclo negativo rende le distanze indefinite
                for (int i = 0; i < n_nodi; i++) { 
                        V_dist[i] = INFTY; // setto INFINITO
                        V_prev[i] = -1; // predecessore non valido
                };

                return;
            }
            elem = elem->next;
        }
    }
}

int DFS(int n) {

    graph_print();

    if (details)
        printf("DFS: lavoro sul nodo %d (visitato %d)\n", n, V_visitato[n]);

    if (V_visitato[n])
        return 0;

    V_visitato[n] = 1; // prima volta che incontro questo nodo

    if (details)
        printf("Visito il nodo %d (val %d)\n", n, V[n]);

    /// esploro la lista di adiacenza
    node_t *elem = E[n]->head;
    while (elem != NULL) { /// elenco tutti i nodi nella lista

        /// espando arco  n --> elem->val
        /// quindi DFS(elem->val)
        output_graph << "dfs_" << n << " -> dfs_" << elem->val;
        if (V_visitato[elem->val])
            output_graph << "[color=gray, label = \"" << ct_visit++ << "\"]";
        else
            output_graph << "[color=red, label = \"" << ct_visit++ << "\"]";
        output_graph << endl;

        if (V_visitato[elem->val] == 1)
            return 1;

        int ret = DFS(elem->val);

        if (ret == 1)
            return 1;

        elem = elem->next;
    }

    V_visitato[n] = 2; // abbandono il nodo per sempre
    return 0;
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int parse_cmd(int argc, char **argv) {
    /// controllo argomenti
    int ok_parse = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'v') {
            details = 1;
            ok_parse = 1;
        }
        if (argv[i][1] == 'g') {
            graph = 1;
            ok_parse = 1;
        }
    }

    if (argc > 1 && !ok_parse) {
        printf("Usage: %s [Options]\n", argv[0]);
        printf("Options:\n");
        printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
        printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {

    if (parse_cmd(argc, argv) != 0) return 1;

    if (graph) {
        output_graph.open("graph.dot");
        output_graph << "digraph g {" << endl;
    }


    int N = 10;
    n_nodi = 6; 

    //// init nodi
    V = new int[n_nodi];
    V_visitato = new int[n_nodi];
    V_prev = new int[n_nodi];
    V_dist = new float[n_nodi];

    //// init archi
    E = new list_t *[n_nodi];

    // costruzione grafo
    for (int i = 0; i < n_nodi; i++) {
        V[i] = 2 * i;
        V_visitato[i] = 0;
        V_prev[i] = -1;
        V_dist[i] = INFTY;
        E[i] = list_new();
        if (i == 0) global_ptr_ref = E[i];
    }

    printf("=== SELEZIONE TEST CASE ===\n");
    printf("1. Test shortestpath (nessun peso negativo)\n");
    printf("2. Test Bellman-Ford con pesi negativi\n");
    printf("3. Test ciclo negativo\n");
    
    int test_case = 3; // SCEGLI case ===================================================
    printf("Eseguendo test case: %d\n\n", test_case);

    switch (test_case) {
        case 1: {
            printf("=== TEST CASE 1: shortestpath con pesi positivi ===\n");
            
            
            list_insert_front(E[0], 1, 4);
            list_insert_front(E[0], 2, 2);
            list_insert_front(E[1], 2, 1);
            list_insert_front(E[1], 3, 5);
            list_insert_front(E[2], 3, 8);
            list_insert_front(E[2], 4, 10);
            list_insert_front(E[3], 4, 2);
            list_insert_front(E[3], 5, 6);
            list_insert_front(E[4], 5, 3);
            
            printf("Grafo creato con solo pesi positivi\n");
            printf("Eseguendo shortestpath...\n");
            shortest_path(0);
            break;
        }
        
        case 2: {
            printf("=== TEST CASE 2: Bellman-Ford con pesi negativi ===\n");
            
            
            list_insert_front(E[0], 1, 1);
            list_insert_front(E[0], 2, 4);
            list_insert_front(E[1], 2, -3);  
            list_insert_front(E[1], 3, 2);
            list_insert_front(E[2], 3, 3);
            list_insert_front(E[1], 4, 2);
            list_insert_front(E[3], 4, -2);  
            list_insert_front(E[4], 5, 1);
            
            printf("Grafo creato con archi di peso negativo:\n");
            printf("- Arco 1->2: peso -3\n");
            printf("- Arco 3->4: peso -2\n");
            printf("Eseguendo Bellman-Ford...\n");
            bellman_ford(0);
            break;
        }
        
        case 3: {
            printf("=== TEST CASE 3: Ciclo negativo ===\n");
            
            list_insert_front(E[0], 1, 1);
            list_insert_front(E[1], 2, -1); 
            list_insert_front(E[2], 3, -1);   
            list_insert_front(E[3], 1, -1); 
            list_insert_front(E[1], 4, 2);
            list_insert_front(E[4], 5, 1);
            
            printf("Grafo creato con ciclo negativo: 1->2->3->1\n");
            printf("- Arco 1->2: peso -1\n");
            printf("- Arco 2->3: peso -1\n");
            printf("- Arco 3->1: peso -1\n");
            printf("Somma ciclo: -3 (negativo!)\n");
            printf("Eseguendo Bellman-Ford...\n");
            bellman_ford(0);
            break;
        }
    }

    graph_print();

    printf("\n=== RISULTATI FINALI ===\n");
    for (int i = 0; i < n_nodi; i++) {
        printf("Nodo %d: ", i);
        if (V_dist[i] < INFTY) {
            printf("distanza = %.2f", V_dist[i]);
            if (V_prev[i] != -1) {
                printf(", precedente = %d", V_prev[i]);
            }
        } else {
            printf("distanza = INFINITO");
        }
        printf("\n");
    }

    // debug
    printf("\n=== STRUTTURA GRAFO ===\n");
    for (int i = 0; i < n_nodi; i++) {
        printf("Nodo %d -> ", i);
        list_print(E[i]);
    }

    if (graph) {
        output_graph << "}" << endl;
        output_graph.close();
        cout << "\nFile graph.dot scritto" << endl
             << "****** Creare il grafo con: neato graph.dot -Tpdf -o graph.pdf" << endl;
    }


    return 0;
}
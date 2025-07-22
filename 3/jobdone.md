# Consegna 3

Partendo dal codice che ricerca la presenza di cicli, estendere il codice per:

* Visitare tutto il grafo, nel caso in cui ci siano parti disconnesse.
* Calcolare la lunghezza del ciclo piu' lungo.
* Bonus: fornire l'elenco dei nodi coinvolti.

## Analisi dell'Implementazione

### 1. Strutture Dati per il Grafo

Per rappresentare il grafo senza usare la STL, abbiamo definito due `struct` fondamentali:

* **`AdjListNode`**: Rappresenta un singolo nodo in una lista di adiacenza.

  ```cpp
  struct AdjListNode {
      int dest;           // Il vertice di destinazione dell'arco
      AdjListNode* next;  // Puntatore al prossimo nodo nella lista
  };
  ```

  Ogni istanza di questa struct corrisponde a un arco uscente da un vertice.

* **`Graph`**: Rappresenta l'intero grafo.

  ```cpp
  struct Graph {
      int V;              // Numero totale di vertici nel grafo
      AdjListNode** adj;  // Un array di puntatori a AdjListNode
  };
  ```

  Il membro `adj` è il cuore della rappresentazione. È un array dinamico la cui dimensione è pari al numero di vertici (`V`). Ogni elemento `adj[i]` è un puntatore alla testa della lista concatenata che contiene tutti i vertici adiacenti al vertice `i`. Se `adj[i]` è `nullptr`, significa che il vertice `i` non ha archi uscenti.

### 2. Procedimento Algoritmico

Il codice è stato esteso per soddisfare tre requisiti principali: visitare grafi disconnessi, trovare il ciclo più lungo e elencarne i nodi.

L'algoritmo identifica il ciclo più lungo trovato tramite i "back edge" rilevati durante la visita. Questo copre i cicli semplici ma non è una soluzione generica per il problema **NP-difficile** del ciclo più lungo.

#### a. Visitare l'Intero Grafo (Componenti Disconnesse)

Questa funzionalità è garantita dall'approccio della DFS. La funzione principale `findLongestCycle` contiene un ciclo che itera su tutti i vertici da `0` a `V-1`:

```cpp
for (int i = 0; i < V; ++i) {
    if (visited[i] == 0) {
        findLongestCycleUtil(i, graph, ...);
    }
}
```

* **Come funziona**: L'array `visited` tiene traccia dei nodi già esplorati. Il ciclo `for` assicura che se il grafo è composto da più parti non collegate tra loro (componenti connesse), la visita DFS venga avviata da un nodo di ciascuna componente.

#### b. Calcolare la Lunghezza del Ciclo Più Lungo

Questa è la logica centrale dell'algoritmo, implementata nella funzione ricorsiva `findLongestCycleUtil`.

* **Rilevamento di un Ciclo**: Un ciclo viene rilevato quando la DFS incontra un vicino `v` che è già nello stack di ricorsione (`visited[v] == 1`). L'arco verso `v` è un "arco all'indietro" (back edge) che chiude un ciclo.
* **Calcolo della Lunghezza**: Una volta trovato un ciclo, si trova l'indice di `v` nel percorso (`path`) corrente per calcolare la lunghezza del ciclo.
* **Mantenere il Massimo**: Una variabile `max_cycle_len` tiene traccia della lunghezza massima trovata finora e viene aggiornata di conseguenza.

#### c. Fornire l'Elenco dei Nodi del Ciclo (Bonus)

* **Salvataggio dei Nodi**: Quando viene trovato un ciclo più lungo di quello massimo corrente, si alloca un nuovo array per memorizzare i nodi del ciclo, copiandoli dal percorso corrente.

### 3. Analisi della Complessità

* **Complessità Temporale**: **O(V + E)**, dove V è il numero di vertici ed E è il numero di archi. Questo è dovuto al fatto che l'algoritmo DFS visita ogni vertice e ogni arco una sola volta.
* **Complessità Spaziale**: **O(V)**. Lo spazio è dominato dall'array `visited` (dimensione V), dall'array `path` (dimensione V) e dalla profondità massima dello stack di ricorsione, che nel caso peggiore è V.

### 4. Gestione della Memoria e Design

* **Gestione Manuale**: Per rispettare il vincolo di non usare la STL, la memoria è gestita manualmente con `new` e `delete[]`.
* **Proprietà della Memoria**: L'algoritmo segue un pattern in cui la funzione `findLongestCycle` alloca memoria per `longest_cycle_nodes`, ma la responsabilità di deallocarla è lasciata al chiamante (`main`). Questo approccio è funzionale ma richiede attenzione per evitare memory leak. In C++ moderno, si preferirebbe usare contenitori RAII (come `std::vector`) che gestiscono la memoria automaticamente.

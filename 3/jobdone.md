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

## Confronto tra DFS Ricorsiva (approccio usato) e Algoritmo di Tarjan

### 1. Approccio usato: DFS Ricorsiva con Array Percorso

Nel codice sviluppato, la ricerca del ciclo più lungo si basa su una DFS ricorsiva che tiene traccia del percorso corrente tramite un array (`path[]`) e una variabile di lunghezza (`path_len`).
Quando durante la visita si incontra un nodo già "in corso di visita" (`visited[v] == 1`), significa che è stato trovato un ciclo:

- Si cerca la posizione di quel nodo nell’array `path[]` e da lì si ricostruisce il ciclo.
- Se il ciclo è più lungo di quelli trovati in precedenza, si aggiorna la lunghezza massima e si salvano i nodi.

**Esempio dal codice:**

```cpp
if (visited[v] == 1) { // Trovato un ciclo
    int cycle_start_index = -1;
    for (int i = 0; i < path_len; ++i) {
        if (path[i] == v) {
            cycle_start_index = i;
            break;
        }
    }
    // Calcola lunghezza e salva nodi del ciclo
}
```

**Caratteristiche:**

- Semplice da implementare.
- Trova il ciclo più lungo, ma non tutte le componenti fortemente connesse (SCC).
- Non usa uno stack esplicito, ma un array percorso.

---

### 2. Algoritmo di Tarjan (SCC)

L’algoritmo di Tarjan trova tutte le componenti fortemente connesse (SCC) di un grafo orientato in tempo lineare.
Utilizza:

- Uno stack esplicito per tenere traccia dei nodi attualmente nel percorso DFS.
- Low-link values per identificare i punti di inizio delle SCC.
- Quando trova una SCC, estrae tutti i nodi dallo stack fino al nodo di partenza della SCC.

**Schema semplificato:**
```cpp
void tarjan(int u) {
    index[u] = lowlink[u] = ++time;
    stack.push(u);
    onStack[u] = true;
    for (v : adj[u]) {
        if (index[v] == -1) {
            tarjan(v);
            lowlink[u] = min(lowlink[u], lowlink[v]);
        } else if (onStack[v]) {
            lowlink[u] = min(lowlink[u], index[v]);
        }
    }
    if (lowlink[u] == index[u]) {
        // Estrai SCC dallo stack
    }
}
```

**Caratteristiche:**

- Trova tutte le SCC (ogni ciclo è una SCC, ma non tutte le SCC sono cicli semplici).
- Usa uno stack esplicito e valori low-link.
- Più complesso, ma più potente per l’analisi strutturale dei grafi.

---

### Differenze principali

| Aspetto                | DFS Ricorsiva (tuo)         | Tarjan (SCC)                |
|------------------------|-----------------------------|-----------------------------|
| Obiettivo              | Ciclo più lungo             | Tutte le SCC                |
| Stack esplicito        | No (usa array percorso)     | Sì                          |
| Low-link               | No                          | Sì                          |
| Complessità            | Più semplice                | Più complesso               |
| Output                 | Un ciclo (il più lungo)     | Tutte le SCC                |
| Applicazione tipica    | Cicli semplici              | Analisi strutturale, SCC    |

---

### Quando usare uno o l’altro?

- **DFS ricorsiva con array percorso**: se vuoi solo sapere se esistono cicli o trovare il ciclo più lungo, questo approccio è semplice ed efficace.
- **Tarjan**: se vuoi trovare tutte le componenti fortemente connesse (ad esempio, per analizzare la struttura globale del grafo), Tarjan è la scelta giusta.

**In sintesi:**
L’approccio usato è perfetto per il problema richiesto dalla consegna (ciclo più lungo). Se dovessi estendere il lavoro all’analisi delle SCC, dovresti implementare Tarjan con stack esplicito e low-link.



## Approfondimento: Algoritmo di Ford-Fulkerson (Flusso Massimo)

L'algoritmo di Ford-Fulkerson è una tecnica fondamentale per risolvere il problema del **max flow** in una rete (grafo orientato con capacità sugli archi). L'obiettivo è trovare la massima quantità di "flusso" che può essere inviata da una sorgente a un pozzo senza superare la capacità degli archi.

### Come funziona
- Si parte con flusso nullo.
- Si cerca un **cammino aumentante** dalla sorgente al pozzo (usando DFS o BFS) lungo il quale è possibile aumentare il flusso.
- Si aumenta il flusso lungo questo cammino della quantità massima possibile (bottleneck).
- Si aggiorna la rete residua e si ripete finché non esistono più cammini aumentanti.

**Pseudocodice semplificato:**
```cpp
while (esiste cammino aumentante da S a T nella rete residua) {
    aumenta il flusso lungo il cammino;
    aggiorna la rete residua;
}
```

### Applicazioni
- Problemi di trasporto, reti di comunicazione, matching bipartito, pianificazione di risorse.

### Confronto con altri algoritmi
- **DFS/BFS**: Ford-Fulkerson usa DFS/BFS come sottoprocedura per trovare cammini aumentanti, ma il suo obiettivo è ottimizzare il flusso, non solo visitare nodi o trovare cicli.
- **Kruskal/Prim (MST)**: Questi algoritmi cercano alberi di copertura a peso minimo, mentre Ford-Fulkerson lavora su capacità e flussi, non su pesi minimi.
- **Tarjan/DFS per cicli**: Questi algoritmi analizzano la struttura dei cicli o delle componenti, Ford-Fulkerson invece trasforma la struttura del grafo per ottimizzare un valore globale (il flusso).

### Sintesi
Ford-Fulkerson mostra come la visita di un grafo (DFS/BFS) possa essere usata in modo avanzato per risolvere problemi di ottimizzazione, diversi dalla semplice esplorazione o dalla ricerca di cicli. È un esempio di come la teoria dei grafi si applichi a problemi pratici di rete e risorse.
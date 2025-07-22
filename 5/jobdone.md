# Consegna 5: Algoritmo di Kruskal

## Descrizione dell'Implementazione

Per questa consegna, è stato implementato l'algoritmo di Kruskal per trovare il Minimum Spanning Tree (MST) di un grafo. L'implementazione si attiene strettamente alla descrizione fornita nel libro di testo **"Introduzione agli Algoritmi" di Cormen et al.** e rispetta il vincolo di non utilizzare librerie esterne o della STL che nascondano la logica fondamentale dell'algoritmo.

### 1. Strutture Dati Fondamentali

A differenza dei task precedenti basati su liste di adiacenza, per l'algoritmo di Kruskal è stata scelta una rappresentazione a **elenco di archi**, più efficiente per questo specifico caso d'uso.

- **`struct Edge`**: Rappresenta un singolo arco del grafo, contenente i vertici di origine (`src`), destinazione (`dest`) e il suo `weight` (peso).
- **`struct Graph`**: Contiene il numero di vertici (`V`), il numero massimo di archi (`E`), il conteggio attuale degli archi (`edge_count`) e un puntatore a un array di `Edge` (`edges`).

### 2. Componenti Chiave dell'Algoritmo

L'intera logica è stata implementata da zero per garantire la piena comprensione di ogni passaggio.

#### Ordinamento degli Archi: `quickSort`

Il primo passo dell'algoritmo di Kruskal richiede di ordinare tutti gli archi del grafo in base al loro peso. Per rispettare i vincoli, è stato implementato l'algoritmo **QuickSort** da zero.

- **`quickSort(Edge arr[], int low, int high)`**: Funzione ricorsiva che ordina l'array di archi.
- **`partition(...)`**: Funzione di supporto che partiziona l'array attorno a un pivot.
- **`swapEdges(...)`**: Funzione ausiliaria per scambiare due archi.

#### Rilevamento dei Cicli: Disjoint Set Union (DSU)

Per determinare se l'aggiunta di un arco creerebbe un ciclo, è stata implementata la struttura dati **Disjoint Set Union (DSU)**, nota anche come Union-Find. Questa struttura è ottimizzata con due tecniche chiave:

1. **`find(Subset subsets[], int i)`**: Trova il rappresentante (radice) dell'insieme a cui un elemento appartiene. Utilizza la **path compression** per appiattire l'albero, rendendo le ricerche future quasi istantanee.
2. **`Union(Subset subsets[], int x, int y)`**: Unisce due insiemi. Utilizza la tecnica **union by rank** per mantenere gli alberi il più bassi possibile, garantendo l'efficienza delle operazioni.

### 3. Funzione Principale: `kruskalMST`

La funzione `kruskalMST` orchestra l'intero processo:

1. **Ordina** l'array di archi del grafo utilizzando la funzione `quickSort` implementata.
2. **Inizializza** la struttura DSU, creando un insieme distinto per ogni vertice.
3. **Itera** attraverso gli archi ordinati, dal più leggero al più pesante.
4. Per ogni arco, utilizza `find` per verificare se i due vertici appartengono già allo stesso insieme.
    - Se **non** appartengono allo stesso insieme, l'arco viene aggiunto all'MST e i due insiemi vengono fusi con `Union`.
    - Se appartengono allo stesso insieme, l'arco viene scartato per evitare cicli.
5. Il processo termina quando l'MST contiene `V-1` archi.
6. Infine, stampa gli archi che compongono l'MST e il suo peso totale.

### 4. Visualizzazione Gerarchica dei Cluster

Per soddisfare la richiesta di analizzare la scomposizione gerarchica, l'algoritmo è stato esteso per mostrare lo stato delle componenti connesse (cluster) a intervalli predefiniti durante la sua esecuzione.

- **`printClusters(Subset subsets[], int V)`**: È stata introdotta una nuova funzione che stampa lo stato attuale dei cluster. Itera su tutti i vertici e raggruppa quelli che appartengono allo stesso insieme (cioè hanno la stessa radice nella struttura DSU).

- **Modifica a `kruskalMST`**: La funzione principale è stata modificata per invocare `printClusters` dopo l'aggiunta di un numero specifico di archi. Questo permette di "fotografare" la formazione dei cluster e osservare come i nodi si aggregano in base agli archi di peso minore, offrendo una visione chiara della coerenza dei dati rappresentati.

### 5. Gestione della Memoria

La memoria per il grafo, gli archi e le strutture DSU è gestita manualmente tramite `new` e `delete[]`, senza fare affidamento su contenitori della STL. La funzione `destroyGraph` assicura che tutta la memoria allocata venga correttamente liberata.

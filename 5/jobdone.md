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

### 5. Scelta e Preparazione del Dataset

Per applicare l'algoritmo a un caso di studio reale, è stato scelto un dataset rappresentante la **rete stradale della città di Bologna**.

- **Fonte:** Il dataset è stato ottenuto da [OpenData Bologna](https://opendata.comune.bologna.it/explore/dataset/rifter_arcstra_li/export/) e pre-elaborato per ottenere un formato `sorgente,destinazione,peso`, dove il peso corrisponde alla lunghezza in metri dell'arco stradale.
- **Caratteristiche:** Il grafo risultante è pesato, non orientato e contiene un numero di nodi e archi sufficientemente grande per un'analisi significativa, superando ampiamente il requisito di 100 nodi.

Per poter utilizzare questo dataset, è stata implementata una funzione `loadGraphFromFile`. Una sfida chiave è stata la gestione degli ID dei nodi, che nel file originale non sono sequenziali (es. `15622`, `5772`). Per risolvere questo problema, è stata utilizzata una `std::map` per creare una mappatura bidirezionale tra gli ID originali e gli indici interni del programma (da `0` a `V-1`), un compromesso necessario per l'interfacciamento con dati reali.

### 6. Analisi Gerarchica e Visualizzazione con Graphviz

Per migliorare l'analisi della scomposizione gerarchica, la visualizzazione è stata potenziata passando da un semplice output testuale a una rappresentazione grafica tramite **Graphviz**.

- **Generazione di File `.dot`**: È stata creata una funzione `generateDotFile` che, a ogni "step" predefinito dell'algoritmo di Kruskal, esporta lo stato corrente dell'MST parziale in un file `.dot`.
- **Contenuto del File `.dot`**: Ogni file generato non solo mostra gli archi dell'MST, ma raggruppa visivamente i nodi all'interno dei rispettivi cluster (componenti connesse), rendendo l'analisi della coesione geografica immediata.
- **Analisi dei Risultati**: Eseguendo il programma, si osserva che i primi cluster a formarsi uniscono nodi geograficamente vicini (strade dello stesso quartiere), come previsto. Man mano che l'algoritmo procede e vengono aggiunti archi con peso maggiore, i cluster si fondono a formare macro-aree, dimostrando che la partizione gerarchica riflette coerentemente la topologia stradale della città.

### 7. Analisi della Coerenza della Partizione Gerarchica

L'esecuzione dell'algoritmo sul dataset della rete stradale di Bologna ha permesso di verificare che la scomposizione gerarchica prodotta è coerente con la natura dei dati.

-   **Dopo 10 archi (le strade più corte)**: L'output mostra la formazione di cluster molto piccoli, spesso composti da sole due o tre nodi. Questo corrisponde alla connessione di incroci stradali estremamente vicini, come le due estremità di una breve via o nodi all'interno della stessa piazza. È il primo livello di aggregazione, basato sulla massima prossimità geografica.

-   **Dopo 50 archi**: I cluster iniziano a fondersi, creando aggregati più grandi. Questo stadio rappresenta la formazione di piccoli "isolati" o "quartieri", dove le strade locali sono state interconnesse. La struttura gerarchica è evidente: i singoli punti si sono uniti in gruppi locali.

-   **Dopo 100 archi**: I cluster sono ancora più estesi e il loro numero totale si riduce. In questa fase, l'algoritmo sta usando strade di collegamento più lunghe (con peso maggiore) per unire i quartieri precedentemente formati. Questo rappresenta la connessione di distretti più ampi della città.

In conclusione, l'analisi dell'output a diversi stadi di esecuzione conferma che **la partizione gerarchica prodotta da Kruskal riflette fedelmente la struttura topologica della rete stradale**. L'algoritmo raggruppa prima i nodi più vicini e poi, in modo gerarchico, unisce questi gruppi per formare aree sempre più vaste, dimostrando una coerenza diretta tra il processo di clustering e la geografia della città.

### [FUN-FACT] Kruskal come Algoritmo di Clustering Gerarchico

L'analogia tra il funzionamento dell'algoritmo di Kruskal e il clustering gerarchico è corretta e molto pertinente. A tutti gli effetti, l'algoritmo di Kruskal può essere visto come un'implementazione di **clustering gerarchico agglomerativo (bottom-up)**.

-   **Bottom-Up (dal basso verso l'alto):** L'algoritmo parte dalla situazione più frammentata possibile (ogni vertice è un cluster a sé) e, passo dopo passo, costruisce una struttura più grande e unificata, fino ad arrivare a un unico cluster che contiene tutti i nodi (l'MST).
-   **Agglomerativo:** Ad ogni iterazione, l'algoritmo "agglomera", ovvero fonde insieme, i due cluster più vicini. La "vicinanza" è definita dal peso dell'arco più leggero che li collega.

Questa corrispondenza è così diretta che Kruskal è l'esatta implementazione di una specifica tecnica di clustering chiamata **single-linkage clustering**, dove la distanza tra due cluster è definita come la distanza minima tra qualsiasi punto di un cluster e qualsiasi punto dell'altro.

| Concetto in Kruskal          | Concetto nel Clustering Gerarchico |
| :--------------------------- | :--------------------------------- |
| Ogni vertice (incrocio)      | Ogni punto dato (data point)       |
| Ogni componente connessa     | Ogni cluster                       |
| Il peso di un arco (distanza) | La distanza tra due punti          |
| Unire due componenti         | Fondere due cluster                |
| L'MST finale                 | Il dendrogramma completo           |

### 8. Gestione della Memoria

La memoria per il grafo, gli archi e le strutture DSU è gestita manualmente tramite `new` e `delete[]`, senza fare affidamento su contenitori della STL. La funzione `destroyGraph` assicura che tutta la memoria allocata venga correttamente liberata.

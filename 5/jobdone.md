# Consegna 5: Algoritmo di Kruskal

Implementare l'algoritmo di Kruskal. Se si usa codice preso altrove, citare la fonte e bisogna essere in grado di descrivere ogni singola riga del programma. Non è possibile usare librerie che nascondono tutto l'algorimo (es. union-find). Scegliere un grafo con almeno 100 nodi con un significato concreto (scaricatelo dalla rete, eventualmente semplificandolo) e applicare l'algoritmo. Mostrare la divisione gerarchica dei primi livelli (con dot oppure su stdout) e osservare se la partizione rappresenta una proprietà coerente con il tipo di dati rappresentati dal grafo

## Descrizione dell'Implementazione

Per questa consegna, è stato implementato l'algoritmo di Kruskal per trovare il Minimum Spanning Tree (MST) di un grafo. L'implementazione si attiene strettamente alla descrizione fornita nel libro di testo **"Introduzione agli Algoritmi" di Cormen et al.** e rispetta il vincolo di non utilizzare librerie esterne o della STL che nascondano la logica fondamentale dell'algoritmo.

### 1. Strutture Dati Fondamentali

A differenza dei task precedenti basati su liste di adiacenza, per l'algoritmo di Kruskal è stata scelta una rappresentazione a **elenco di archi**, più efficiente per questo specifico caso d'uso.

- **`struct Edge`**: Rappresenta un singolo arco del grafo, contenente i vertici di origine (`src`), destinazione (`dest`) e il suo `weight` (peso).
- **`struct Graph`**: Contiene il numero di vertici (`V`), il numero massimo di archi (`E`), il conteggio attuale degli archi (`edge_count`) e un puntatore a un array di `Edge` (`edges`).

### 2. Componenti Chiave dell'Algoritmo

L'intera logica è stata implementata da zero per garantire la piena comprensione di ogni passaggio.

#### Ordinamento degli Archi: `quickSort`

- **`quickSort(Edge arr[], int low, int high)`**: Funzione ricorsiva che ordina l'array di archi.
- **`partition(...)`**: Funzione di supporto che partiziona l'array attorno a un pivot.
- **`swapEdges(...)`**: Funzione ausiliaria per scambiare due archi.

#### Rilevamento dei Cicli: Disjoint Set Union (DSU)

Per determinare se l'aggiunta di un arco creerebbe un ciclo, è stata implementata la struttura dati **Disjoint Set Union (DSU)**, nota anche come Union-Find. Questa struttura è ottimizzata con due tecniche chiave:

### 3. Funzione Principale: `kruskalMST`

La funzione `kruskalMST` orchestra l'intero processo:

1. **Ordina** l'array di archi del grafo utilizzando la funzione `quickSort` implementata.
2. **Inizializza** la struttura DSU, creando un insieme distinto per ogni vertice.
     - **`rank` nella DSU**: Il campo `rank` nella struttura `Subset` rappresenta una stima della profondità dell'albero che identifica ciascun insieme disgiunto. Serve per ottimizzare l'operazione di unione (union by rank): quando si fondono due insiemi, quello con rank minore viene attaccato come sottoalbero di quello con rank maggiore. Questo mantiene gli alberi più piatti, riducendo la complessità delle operazioni di ricerca e unione nella DSU.
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

- **Caratteristiche:** Il grafo risultante è pesato, non orientato e contiene un numero di nodi e archi sufficientemente grande per un'analisi significativa, superando ampiamente il requisito di 100 nodi.

#### Gestione degli ID dei nodi: CustomMap

La gestione della corrispondenza tra gli identificatori reali dei nodi (ad esempio, 5772, 15610) e gli indici interni (0..V-1) necessari per l'algoritmo è stata implementata tramite una struttura custom chiamata `CustomMap`.

- Tradurre ogni ID reale in un indice interno sequenziale (per l'algoritmo)
- Tradurre ogni indice interno nell'ID reale (per l'output)

  - È una semplice tabella lineare di coppie chiave-valore, con ricerca e inserimento O(N) (dove N è il numero di nodi distinti). Non usa hash né alberi bilanciati.
  - *Limiti*: La struttura non è adatta a grafi molto grandi (es. decine di migliaia di nodi), poiché la ricerca lineare rallenta sensibilmente le operazioni. L'assenza di gestione dinamica della capacità e la mancanza di ottimizzazioni (come hash o alberi) la rendono inadatta a scenari reali ad alte prestazioni.

### 6. Analisi Gerarchica e Visualizzazione con Graphviz

- **Generazione di File `.dot`**: È stata creata una funzione `generateDotFile` che, a ogni "step" predefinito dell'algoritmo di Kruskal, esporta lo stato corrente dell'MST parziale in un file `.dot`.
- **Contenuto del File `.dot`**: Ogni file generato non solo mostra gli archi dell'MST, ma raggruppa visivamente i nodi all'interno dei rispettivi cluster (componenti connesse), rendendo l'analisi della coesione geografica immediata.
- **Analisi dei Risultati**: Eseguendo il programma, si osserva che i primi cluster a formarsi uniscono nodi geograficamente vicini (strade dello stesso quartiere), come previsto. Man mano che l'algoritmo procede e vengono aggiunti archi con peso maggiore, i cluster si fondono a formare macro-aree, dimostrando che la partizione gerarchica riflette coerentemente la topologia stradale della città.

### 7. Analisi della Coerenza della Partizione Gerarchica

L'esecuzione dell'algoritmo sul dataset della rete stradale di Bologna ha permesso di verificare che la scomposizione gerarchica prodotta è coerente con la natura dei dati.

- **Dopo 50 archi**: I cluster iniziano a fondersi, creando aggregati più grandi. Questo stadio rappresenta la formazione di piccoli "isolati" o "quartieri", dove le strade locali sono state interconnesse. La struttura gerarchica è evidente: i singoli punti si sono uniti in gruppi locali.

- **Dopo 100 archi**: I cluster sono ancora più estesi e il loro numero totale si riduce. In questa fase, l'algoritmo sta usando strade di collegamento più lunghe (con peso maggiore) per unire i quartieri precedentemente formati. Questo rappresenta la connessione di distretti più ampi della città.

In conclusione, l'analisi dell'output a diversi stadi di esecuzione conferma che **la partizione gerarchica prodotta da Kruskal riflette fedelmente la struttura topologica della rete stradale**. L'algoritmo raggruppa prima i nodi più vicini e poi, in modo gerarchico, unisce questi gruppi per formare aree sempre più vaste, dimostrando una coerenza diretta tra il processo di clustering e la geografia della città.

### Kruskal e il Clustering Gerarchico

L'algoritmo di Kruskal può essere interpretato come una forma di **clustering gerarchico agglomerativo (bottom-up)**:

- **Bottom-Up (dal basso verso l'alto):** L'algoritmo parte dalla situazione più frammentata possibile (ogni vertice è un cluster a sé) e, passo dopo passo, costruisce una struttura più grande e unificata, fino ad arrivare a un unico cluster che contiene tutti i nodi (l'MST).
- **Agglomerativo:** Ad ogni iterazione, l'algoritmo "agglomera", ovvero fonde insieme, i due cluster più vicini. La "vicinanza" è definita dal peso dell'arco più leggero che li collega.

Questa corrispondenza è diretta: Kruskal è l'implementazione di una tecnica di clustering chiamata **single-linkage clustering**, dove la distanza tra due cluster è la distanza minima tra qualsiasi punto di un cluster e qualsiasi punto dell'altro.

| Concetto in Kruskal          | Concetto nel Clustering Gerarchico |
| :--------------------------- | :--------------------------------- |
| Il peso di un arco (distanza) | La distanza tra due punti          |
| Unire due componenti         | Fondere due cluster                |
| L'MST finale                 | Il dendrogramma completo           |

### 8. Gestione della Memoria

La memoria per il grafo, gli archi e le strutture DSU è gestita manualmente tramite `new` e `delete[]`, senza fare affidamento su contenitori della STL. La funzione `destroyGraph` assicura che tutta la memoria allocata venga correttamente liberata.

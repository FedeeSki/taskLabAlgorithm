# Consegna 4

Estendere il codice di base di shortest path, sostituendo la gestione per l'estrazione del minimo nodo (attualmente con un ciclo O ( n ) ), con l'heap utilizzato nelle lezioni precedenti (lezione11-heap File).

Il punto di attenzione principale e' di garantire la complessita' log ( n )  e di recuperare correttamente l'indice dell'array dell'heap a partire dal codice del nodo (ad esempio nella decrease_key).

Assicurarsi di non confondere l'indice del nodo del grafo con l'indice della posizione dell'elemento nell'heap. NON usare implementazione di heap alternative.

Studiare e implementare l'algoritmo di Bellman-Ford, sfruttando il codice di shortest path già scritto. Provare con un esempio di grafo con almeno un arco di peso negativo e un altro grafo con un ciclo di archi di peso tutti negativi.

Opzionale: adattare le visualizzazioni dei grafi con dot per mostrare il suo corretto funzionamento.

## Analisi dell'implementazione

# Consegna 2

Scrivere le funzioni per ottenere i seguenti output (in input un albero binario):

- Flip albero: per ogni nodo il sottoalbero destro diventa il sottoalbero sinistro e viceversa.
- Calcolo profondità di ciascun nodo dell'albero
- Funzione isBalanced: restituire un flag che indichi se l'albero è bilanciato o meno. - Bonus: l'algoritmo è O ( n ) con n nodi
- Funzione isComplete: restituire un flag che indiche se l'albero è completo secondo la definizione classica
- Funzione Lowest Common Ancestor: dati due valori presenti nell'albero, restituire il valore del nodo piu' basso che contiene entrambi nel suo sottoalbero

## Diario di bordo

1. **Struttura base dell'albero binario**
   - Creata struct `T_Node` per i nodi
   - Implementata classe `B_Tree` per gestire l'albero
   - Costruttore e distruttore con gestione memoria

2. **Funzioni di input**
   - `readTreeFromInput()`: inserimento manuale dell'albero
   - `generateRandomTree()`: generazione automatica con parametri:
     - Profondità massima dell'albero
     - Range di valori (min, max)

3. **Visualizzazione**
   - Chiesto di farlo ad un LLM Claude 4.0.
   - Traversal in-order per mostrare i nodi ordinati
   - `generateDotFile()`: esportazione in formato DOT per Graphviz
   - Menu interattivo per scegliere modalità di input/output

## Comandi per visualizzazione grafica

### Compilazione

```bash
g++ consegena2_bTree.cpp -o bTree
```

### Esecuzione

```bash
./bTree
```

### Generazione immagine da file DOT (richiede Graphviz)

```bash
# Installa Graphviz (se non presente)
brew install graphviz  # macOS
# sudo apt install graphviz  # Linux
# choco install graphviz  # Windows

# Genera immagine PNG
dot -Tpng albero.dot -o albero.png

# Altri formati disponibili
dot -Tsvg albero.dot -o albero.svg
dot -Tpdf albero.dot -o albero.pdf
```

### 1. Flip Albero

**Ragionamento e implementazione:**

La funzione flip deve scambiare ricorsivamente i sottoalberi sinistro e destro di ogni nodo.

**Approccio scelto:**

- **Funzione helper ricorsiva**: `flipTreeHelper(T_Node* node)`
- **Caso base**: Se il nodo è `nullptr`, non fare nulla
- **Operazione**: Scambiare i puntatori `left` e `right` del nodo corrente
- **Ricorsione**: Applicare lo stesso processo ai sottoalberi

**Complessità:**

- Tempo: O(n) - visita ogni nodo una volta
- Spazio: O(h) - stack ricorsivo profondo quanto l'altezza dell'albero

### 2. Profondità per ogni nodo dell'albero

**Ragionamento e implementazione:**

La funzione deve calcolare e mostrare la profondità di ogni nodo dell'albero, dove la radice ha profondità 0.

**Approccio scelto:**

- **Funzione helper ricorsiva**: `printNodeDepthsHelper(T_Node* node, int currentDepth)`
- **Caso base**: Se il nodo è `nullptr`, non fare nulla
- **Operazione**: Stampare il nodo corrente con la sua profondità
- **Ricorsione**: Visitare ricorsivamente i sottoalberi incrementando la profondità di 1

**Complessità:**

- Tempo: O(n) - visita ogni nodo una volta
- Spazio: O(h) - stack ricorsivo profondo quanto l'altezza dell'albero

### 3. Verifica bilanciamento dell'albero (isBalanced) [fare in O(n)]

**Ragionamento e implementazione:**

Un albero binario è bilanciato se per ogni nodo la differenza di altezza tra sottoalbero sinistro e destro è al massimo 1. L'implementazione deve essere efficiente O(n).

Calcolo + verifica in un solo passo: Non separiamo il calcolo dell'altezza dalla verifica del bilanciamento
Propagazione intelligente: Usiamo il valore -1 come "flag di errore" che si propaga immediatamente verso l'alto
Early exit: Appena troviamo un sottoalbero sbilanciato, fermiamo tutto senza continuare calcoli inutili
Riuso dei risultati: L'altezza calcolata durante la verifica viene riutilizzata per il nodo padre

**Approccio scelto:**

Ottimizzazioni implementate:

1. Short-circuit evaluation: if (leftHeight == -1) return -1;
2. Single-pass algorithm: Una sola traversata dell'albero
3. Memory efficient: Usa lo stack di ricorsione, niente strutture dati aggiuntive

Questo è un esempio classico di programmazione dinamica applicata agli alberi: invece di ricalcolare le stesse informazioni più volte, le calcoliamo una volta e le propaghiamo verso l'alto!

- **Funzione helper ricorsiva**: `isBalancedHelper(T_Node* node)` che ritorna l'altezza del sottoalbero o -1 se non bilanciato
- **Caso base**: Se il nodo è `nullptr`, ritorna 0 (altezza di albero vuoto)
- **Operazione**:
  1. Calcola ricorsivamente l'altezza del sottoalbero sinistro
  2. Se sinistro non bilanciato (ritorna -1), propaga il -1
  3. Calcola ricorsivamente l'altezza del sottoalbero destro
  4. Se destro non bilanciato (ritorna -1), propaga il -1
  5. Verifica se la differenza di altezza è ≤ 1
  6. Se sì, ritorna l'altezza corrente; se no, ritorna -1
- **Ottimizzazione**: Il controllo avviene durante la visita, senza bisogno di ricalcolare le altezze

**Complessità:**

- Tempo: O(n) - ogni nodo visitato una sola volta
- Spazio: O(h) - stack ricorsivo profondo quanto l'altezza dell'albero

### 4. Verifica se l'albero è completo (isComplete)

**Ragionamento e implementazione:**

Un albero binario è completo se tutti i suoi livelli, eccetto eventualmente l'ultimo, sono completamente riempiti e se tutti i nodi dell'ultimo livello sono il più a sinistra possibile.

**Approccio scelto:**

L'approccio si basa sull'indicizzazione dei nodi come se fossero in un array (heap-like).

- Radice: indice 0
- Figlio sinistro di un nodo `i`: `2*i + 1`
- Figlio destro di un nodo `i`: `2*i + 2`

1. **Conteggio nodi**: Per prima cosa, si calcola il numero totale di nodi `n` nell'albero con una visita preliminare (O(n)).
2. **Funzione helper ricorsiva**: `isCompleteHelper(T_Node* node, int index, int n)`
3. **Caso base**: Se il nodo è `nullptr`, il percorso è valido (ritorna `true`).
4. **Verifica indice**: Se l'indice `index` del nodo corrente è maggiore o uguale a `n`, significa che c'è un "buco" nella struttura, quindi l'albero non è completo (ritorna `false`).
5. **Ricorsione**: Si richiama la funzione sui figli sinistro e destro, passando i loro indici calcolati. L'albero è completo solo se entrambe le chiamate ricorsive ritornano `true`.

**Complessità:**

- Tempo: O(n) - richiede due visite. Una per contare i nodi e una per verificare la completezza.
- Spazio: O(h) - stack ricorsivo profondo quanto l'altezza dell'albero

### 5. Lowest Common Ancestor (LCA)

**Ragionamento e implementazione:**

Dati due nodi, l'LCA è il nodo più profondo nell'albero che li ha entrambi come discendenti.

**Approccio scelto**: Path-based

La logica è trovare i percorsi dalla radice a ciascuno dei due nodi e poi trovare l'ultimo nodo in comune in questi due percorsi.

1. **Subroutine `findPath()`**:
   - **Scopo**: Trovare e memorizzare il percorso da un nodo radice a un nodo target.
   - **Implementazione**: Funzione ricorsiva che esegue un attraversamento. Aggiunge il nodo corrente al percorso e, se è il target, ritorna `true`. Altrimenti, cerca ricorsivamente nei sottoalberi sinistro e destro. Se il target non viene trovato in nessun sottoalbero, il nodo corrente viene rimosso dal percorso (backtracking) e la funzione ritorna `false`.
   - **Parametri**: Usa un array di puntatori a nodi e un intero passato per riferimento per tenere traccia della lunghezza del percorso.

2. **Funzione `findLCAHelper()`**:
   - **Operazione**:
     1. Chiama `findPath()` due volte per ottenere i percorsi per i due valori dati.
     2. Se uno dei due percorsi non viene trovato, significa che uno dei nodi non è nell'albero, quindi ritorna `nullptr`.
     3. Scorre entrambi gli array dei percorsi contemporaneamente con un indice `i`.
     4. Si ferma quando i nodi ai due percorsi all'indice `i` non corrispondono più.
     5. L'LCA è il nodo all'indice `i-1`, ovvero l'ultimo nodo in comune.
   - **Limitazione**: Per evitare di usare la STL, si utilizzano array C-style di dimensione fissa (`MAX_NODES = 100`), assumendo che l'albero non superi questa altezza.

**Complessità:**

- Tempo: O(n) - nel caso peggiore, `findPath` deve visitare tutti i nodi. L'operazione viene eseguita due volte, e il confronto richiede O(h). Il totale è dominato da O(n).
- Spazio: O(h) - per memorizzare i due percorsi, la cui lunghezza massima è l'altezza dell'albero

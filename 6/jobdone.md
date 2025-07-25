

# Consegna 6

**Testo della consegna:**

Adattare l'algoritmo di Needleman-Wunsch per ottenere un confronto tra versioni diverse di un codice sorgente. Preparare un test con un file C++ da almeno 100 righe. L'obiettivo è quello di dare priorità alla struttura dei blocchi del codice e poi ai token. Gli spazi/tab invece possono essere aggiunti o tolti senza penalità. L'output deve indicare i cambiamenti effettuati, come lista di azioni o stringa di cambiamento (- per cancellazione e caratteri aggiunti o modificati).

---

## Come la soluzione rispetta la consegna

- **Adattamento di Needleman-Wunsch:**
  - L'algoritmo classico viene applicato non su caratteri, ma su sequenze di token estratti dal codice sorgente.

- **Priorità alla struttura dei blocchi:**
  - Il sistema di punteggio assegna il valore massimo ai match tra simboli strutturali (`{`, `}`, `(`, `)`, `;`), come richiesto.
  - Le keyword di controllo (`if`, `for`, ecc.) hanno priorità intermedia.
  - Tutti gli altri token (variabili, numeri, operatori) hanno priorità minore.

- **Indifferenza a spazi/tab:**
  - Il tokenizer ignora completamente spazi, tabulazioni e a capo: due file che differiscono solo per la formattazione produrranno la stessa sequenza di token.

- **Output richiesto:**
  - L'output dell'allineamento è una lista di azioni: MATCH, CHANGE (sostituzione), DELETE (cancellazione), INSERT (aggiunta), come richiesto dalla consegna.

- **Test realistico:**
  - I file di test (`file1.cpp` e `file2.cpp`) sono versioni diverse di un programma C++ realistico, con almeno 100 righe e modifiche di tipo lessicale, strutturale e funzionale.

---


## Scelte Implementative Attuali


La versione attuale implementa un **tokenizer semplice** (splitter), come richiesto dalla consegna:

- Il file viene letto carattere per carattere.
- Gli spazi vengono ignorati.
- Le sequenze di lettere/cifre/underscore sono token (identificatori, keyword, numeri).
- Tutti gli altri caratteri (simboli come `{`, `}`, `;`, `+`, ecc.) sono token singoli.
- Non vengono gestiti commenti, stringhe, operatori multi-carattere o casi particolari.

**Esempio:**

Il codice:

```cpp
if (x > 0) { // Controlla se x è positivo
  y = 1;
}
```

Diventa la sequenza di token:
`["if", "(", "x", ">", "0", ")", "{", "y", "=", "1", ";", "}"]`

Questo approccio permette di confrontare due versioni di codice ignorando la formattazione e gli spazi.


### Sistema di punteggio

Il sistema di punteggio è gerarchico:

- **Match Strutturale (10):** `{`, `}`, `(`, `)`, `;`, ecc.
- **Match Keyword (5):** `if`, `for`, `while`, ecc.
- **Match generico (2):** variabili, numeri, operatori.
- **Mismatch (-2)**
- **Gap (-3)**

## Possibili Miglioramenti Futuri

---

## Spiegazione dettagliata delle funzioni principali (con esempi dal codice)

### 1. Tokenizzazione (`tokenize`)

**Funzione:**
```cpp
Vector<Token> tokenize(const char* filename)
```
**Descrizione:**
Legge un file sorgente carattere per carattere e lo trasforma in una sequenza di token, ignorando spazi e tabulazioni. Ogni token è classificato come KEYWORD, IDENTIFIER, NUMBER, OPERATOR o SYMBOL.

**Esempio dal codice:**
Supponiamo che il file contenga:
```cpp
if (x > 0) { y = 1; }
```
La funzione produrrà i token:
`[KEYWORD:if] [SYMBOL:(] [IDENTIFIER:x] [OPERATOR:>] [NUMBER:0] [SYMBOL:)] [SYMBOL:{] [IDENTIFIER:y] [OPERATOR:=] [NUMBER:1] [SYMBOL:;] [SYMBOL:}]`

**Punti chiave:**
- Usa funzioni personalizzate come `my_isspace`, `my_isalpha`, `my_isdigit` per classificare i caratteri.
- Riconosce le keyword tramite la funzione `is_keyword`.
- Tutti i simboli singoli (es. `{`, `;`, `}`) sono token a sé.

---

### 2. Sistema di punteggio (`score`)

**Funzione:**
```cpp
int score(const Token& a, const Token& b)
```
**Descrizione:**
Assegna un punteggio a una coppia di token, dando priorità ai simboli strutturali, poi alle keyword, poi agli altri token. Penalizza mismatch e gap.

**Esempio dal codice:**
Se confrontiamo `Token(SYMBOL, ";")` con `Token(SYMBOL, ";")` il punteggio sarà 10 (match strutturale).
Se confrontiamo `Token(KEYWORD, "if")` con `Token(KEYWORD, "for")` il punteggio sarà -2 (mismatch).

**Punti chiave:**
- Il punteggio guida l'allineamento ottimale nell'algoritmo Needleman-Wunsch.

---

### 3. Needleman-Wunsch (`needlemanWunsch`)

**Funzione:**
```cpp
void needlemanWunsch(const Vector<Token>& seq1, const Vector<Token>& seq2)
```
**Descrizione:**
Applica l'algoritmo di Needleman-Wunsch alle due sequenze di token, costruendo una matrice di punteggi e poi eseguendo il backtracking per produrre la lista di azioni (MATCH, CHANGE, DELETE, INSERT).

**Esempio dal codice:**
Se `seq1` contiene `[a, =, 1, ;]` e `seq2` contiene `[a, =, 2, ;]`, l'output sarà:
```
MATCH: ;
CHANGE: 1 -> 2
MATCH: =
MATCH: a
```
(l'ordine è inverso perché il backtracking parte dal fondo)

**Punti chiave:**
- La matrice viene costruita manualmente usando la classe `Vector` custom.
- Il backtracking determina la sequenza ottimale di azioni per trasformare una sequenza nell'altra.

---

### 4. Strutture dati custom (`String` e `Vector`)

**String:**
```cpp
class String { ... }
```
Gestisce stringhe dinamiche senza usare <string> o <cstring>. Implementa metodi come `push_back`, `custom_strlen`, `custom_strcpy`, `custom_strcmp`.

**Vector:**
```cpp
template <typename T> class Vector { ... }
```
Gestisce array dinamici generici, con crescita automatica e gestione della memoria secondo la regola del tre (costruttore di copia, assegnazione, distruttore).

**Esempio dal codice:**
Quando si tokenizza un file, i token vengono inseriti in un `Vector<Token>`, che cresce automaticamente all'occorrenza.

---

### 5. Esempio pratico di output

Supponiamo che in `file1.cpp` ci sia:
```cpp
Graph my_graph(8);
my_graph.add_edge(0, 1);
```
e in `file2.cpp`:
```cpp
CoolGraph my_cool_graph(10);
my_cool_graph.add_edge(0, 1);
```
L'output conterrà:
```
CHANGE: Graph -> CoolGraph
CHANGE: my_graph -> my_cool_graph
CHANGE: 8 -> 10
MATCH: ;
MATCH: .
MATCH: add_edge
MATCH: (
MATCH: 0
MATCH: ,
MATCH: 1
MATCH: )
MATCH: ;
```
Questo mostra chiaramente come il programma individua sia le modifiche lessicali che quelle strutturali.

- Implementare un vero lexer che riconosca commenti, stringhe, operatori multi-carattere e casi particolari del C++.
- Ottimizzare la gestione della memoria e delle strutture dati.
- Introdurre una gestione più raffinata dei token per ridurre i falsi positivi nei match.

### Dal Percorso all'Output - Il Backtracking

Una volta riempita la matrice, si esegue il backtracking dall'angolo in basso a destra. Il percorso scelto ci dice esattamente cosa è successo:

- **Movimento Diagonale:** Indica un allineamento.
  - Se i token erano uguali, è un **match**. L'output mostra il token così com'è (es. `if`).
  - Se i token erano diversi, è un **mismatch**. L'output può indicare la sostituzione (es. `5 -> 10`).
- **Movimento Verticale:** Indica un'**inserzione** nella seconda sequenza (un token presente nel file nuovo ma non nel vecchio). L'output mostra un'aggiunta (es. `[+ nuovo_token]`).
- **Movimento Orizzontale:** Indica una **cancellazione** dalla prima sequenza (un token presente nel file vecchio ma non nel nuovo). L'output mostra una rimozione (es. `[- vecchio_token]`).

**Output Finale Esempio:**
Per le sequenze `if (x > 5)` e `if (x > 10)`, il backtracking produrrebbe un allineamento simile a:
`if ( x > 5->10 )`

Per un cambiamento più complesso come da `a=1;` a `a=1; b=2;`, l'output sarebbe:
`a = 1 ; [+ b] [+=] [+=] [2] [+ ;]`

Questo approccio trasforma Needleman-Wunsch da un semplice allineatore di sequenze a uno strumento potente per l'analisi sintattica delle differenze tra codici sorgente.

### Scelta e Idoneità dei File di Test (`file1.cpp` e `file2.cpp`)

Per validare l'efficacia dell'algoritmo di Needleman-Wunsch adattato, sono stati scelti due file, `file1.cpp` e `file2.cpp`, che non rappresentano semplici variazioni casuali, ma simulano un **realistico scenario di refactoring del codice**. Questa scelta è strategica per le seguenti ragioni:

1. **Varietà delle Modifiche**: I due file includono un'ampia gamma di cambiamenti tipici dello sviluppo software:
   - **Refactoring Lessicale**: Nomi di classi, variabili e funzioni sono stati rinominati (es. `Graph` -> `CoolGraph`, `num_vertices` -> `v_count`). Questo testa la capacità dell'algoritmo di gestire i "mismatch".
   - **Aggiunta di Funzionalità**: `file2.cpp` introduce il metodo `count_edges()`, un intero blocco di codice che l'algoritmo deve identificare come una serie di "inserzioni".
   - **Modifiche Logiche e Strutturali**: La logica nel `main` è stata alterata, cambiando i parametri e l'ordine delle chiamate. Questo mette alla prova la gestione di cambiamenti localizzati e strutturali.

2. **Validazione del Sistema di Punteggio**: Le differenze presenti sono ideali per testare la robustezza del sistema di punteggio personalizzato. Ad esempio, confrontando un nome di variabile rinominato, l'algoritmo deve correttamente preferire un "mismatch" (costo basso) a una più costosa operazione di "cancellazione + inserzione", dimostrando che la logica di penalità funziona come previsto.

3. **Conformità ai Requisiti**: I file permettono di verificare un requisito chiave della consegna: l'indifferenza alla formattazione. Modificando solo spazi o a capo in uno dei file, si può confermare che il tokenizer produce la stessa sequenza di token, risultando in un punteggio di allineamento perfetto.

In sintesi, `file1.cpp` e `file2.cpp` costituiscono un caso di test eccellente perché contengono le esatte sfide lessicali e strutturali per cui l'algoritmo è stato progettato, garantendo una validazione completa e significativa della soluzione.



### Strutture Dati e Funzioni di Utilità Personalizzate

Per rispettare i vincoli didattici e mostrare padronanza della gestione della memoria, il progetto **non usa** `<string>`, `<vector>`, `<cstring>`, `<cctype>`, ma implementa versioni semplici e dedicate:

- **`String`**: array dinamico di caratteri, con `push_back` efficiente (raddoppio capacità). Funzioni base (`custom_strlen`, `custom_strcpy`, `custom_strcmp`) per evitare dipendenze da `<cstring>`.
- **`Vector<T>`**: vettore dinamico generico, con crescita esponenziale e gestione della memoria secondo la "regola del tre" (distruttore, copia, assegnazione).
- **Funzioni carattere**: `my_isspace`, `my_isalpha`, `my_isdigit` sostituiscono `<cctype>` con semplici controlli su valori ASCII.

Queste scelte rendono il codice didattico, portabile e facilmente spiegabile.


### Implementazione dell'Algoritmo

L'implementazione si basa su tre componenti principali:

1. **`tokenize(const char* filename)`**:
   - **Ragionamento**: Il tokenizer è volutamente semplice, simile a uno splitter: legge carattere per carattere, ignora spazi, raggruppa lettere/cifre in token, e tratta ogni simbolo come token singolo.
   - **Funzionamento**:
     - Ignora spazi bianchi (`my_isspace`).
     - Raggruppa sequenze di lettere/cifre/underscore in token:
         - Se il token corrisponde a una keyword C++ (`is_keyword`), viene classificato come `KEYWORD`.
         - Se è solo numerico, viene classificato come `NUMBER`.
         - Altrimenti è un `IDENTIFIER`.
     - Ogni altro carattere (simboli) è un token a sé (es. `{`, `}`, `;`, `+`, ecc.):
         - Se il simbolo è tra `{ } ( ) [ ] ;` viene classificato come `SYMBOL`, altrimenti come `OPERATOR`.
     - Non gestisce commenti, stringhe, operatori multi-carattere o casi particolari: scelta voluta per semplicità e chiarezza.

2. **`score(const Token& a, const Token& b)`**:
   - **Ragionamento**: Sistema di punteggio gerarchico, come da consegna.
   - **Funzionamento**:
     - Se i token sono identici, assegna punteggio in base al tipo (`SYMBOL` 10, `KEYWORD` 5, altri 2).
     - Se diversi, penalità fissa (-2).
     - Penalità di gap (-3) applicata nella matrice.

3. **`needlemanWunsch(const Vector<Token>& seq1, const Vector<Token>& seq2)`**:
   - **Ragionamento**: Implementazione classica, con matrice, riempimento e backtracking.
   - **Funzionamento**:
     - Crea e riempie la matrice dei punteggi.
     - Esegue il backtracking per produrre la lista di azioni (MATCH, CHANGE, DELETE, INSERT).



### Aspetti Teorici e Motivazione

L'obiettivo era realizzare un programma **semplice da spiegare**, ma che rispettasse la consegna e producesse un output utile e leggibile. In particolare:

- Il tokenizer è volutamente minimale: non gestisce casi particolari, ma riconosce keyword, numeri e simboli, che sono sufficienti per la maggior parte dei confronti tra versioni di codice.
- Le strutture dati custom sono didattiche e mostrano la comprensione della gestione della memoria.
- La logica di scoring e backtracking è classica e facilmente illustrabile.

**Teoria:**
L'algoritmo di Needleman-Wunsch, nato per l'allineamento di sequenze biologiche, si adatta perfettamente al confronto tra sequenze di token di codice. La matrice di punteggio e il backtracking permettono di individuare in modo ottimale match, cambiamenti, inserimenti e cancellazioni tra due versioni di un file.

**Nota:** Un'implementazione più avanzata (con vero lexer, gestione commenti/stringhe/operatori, lookahead/ungetc, ecc.) è possibile e auspicabile per progetti futuri o per esigenze di confronto più sofisticate, ma non è necessaria per questa consegna.

## Miglioramenti Possibili (versione avanzata)

Se richiesto un confronto ancora più robusto e preciso, si potrebbero implementare:

- Un vero lexer che riconosca e scarti commenti (`//`, `/* ... */`), gestisca stringhe e caratteri letterali, e operatori multi-carattere (`==`, `++`, `->`, ecc.).
- Un sistema di lookahead (es. con `ungetc`) per una tokenizzazione più precisa.
- Una classificazione più raffinata dei token (es. distinguere tra tipi, costanti, macro, ecc.).
- Un sistema di punteggio ancora più gerarchico e personalizzabile.
- Ottimizzazioni sulle strutture dati e sulla gestione della memoria.

Questi miglioramenti erano stati implementati in una versione precedente, ma sono stati rimossi per privilegiare la semplicità e la chiarezza, in linea con i requisiti della consegna.

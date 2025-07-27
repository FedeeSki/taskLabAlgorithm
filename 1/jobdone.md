# Consegna completa da elly

La consegna prevede di sviluppare, partendo dal file consegna-loader.cpp, un algoritmo di sorting dedicato per la serie di dati data.txt (100 serie da 1000 elementi ciascuna).
I due file si trovano nel materiale didattico nella cartella consegna1. Le valutazioni vanno fatte su data.txt e non su altre istanze.

In particolare si deve utilizzare o creare un algoritmo di ordinamento SENZA basarsi su algoritmi visti a lezione (si puo' eventualmente usare solo la funzione merge di mergesort). Per consultare una lista di idee guardare qui. L'obiettivo e' quello di minimizzare le letture in memoria RAM durante l'ordinamento delle serie di dati. Fate attenzione a misurare correttamente gli accessi in memoria: un accesso ad una variabile (registro) è gratis, mentre la lettura di un array costa 1.

L'algoritmo deve migliorare le letture medie di quicksort (della versione di base nel file).

Potete osservare le proprietà dei dati e selezionare l'algoritmo piu' adatto.

Consegnare il file cpp (consegna1.cpp) e un file di testo (consegna1.txt) in cui si riporta l'output del programma (numero di letture in memoria per ciascuna serie e i totali, cosi' come già preparato in output dal prototipo).

Tra le consegne effettuate entro il 31 Marzo verra' selezionata quella piu' efficiente, che verra' discussa in aula.

Ricordo di specificare nel file sorgente l'eventuale gruppo di lavoro.

## Come ho affrontato il problema

- Letto il codice
- Runnato il codice e guardato i risultati:
  **min: 70053, med: 90000, max: 111720**
- Letture ridondanti:
  1. Legge il pivot: 1 lettura
  2. Per ogni elemento nel loop: 1 lettura per il confronto
  3. Per ogni swap: 2 letture aggiuntive
  4. Swap finale: 2 letture

## Analisi dei dati (data.txt)

- Analisi dei dati, ho dato in pasto ad un LLM Claude Sonnet 4, **data.txt** e me li sono fatti analizzare notando che:

  Componente sinusoidale: 5000*(1+sin(π*(i/(n/2.0))))

  Crea un pattern sinusoidale che va da 0 a 10000.

  Il seno va da -1 a +1, quindi (1+sin) va da 0 a 2.

  Moltiplicato per 5000 = range 0-10000.

  Rumore casuale: rand()%(1+(int)(n*pow(i/n, 2)))

  Aggiunge rumore che cresce quadraticamente con la posizione.
  All'inizio (i=0): rumore minimo
  Alla fine (i=999): rumore massimo

## Analisi dati reali

Guardando i campioni di data.txt, tutti iniziano con 5000 e seguono un pattern molto regolare:

```bash
5000,5031,5062,5094,5125,5157,5188,5219,5251,5282,5313,5345...
```

Caratteristiche Scoperte:

1. **QUASI ORDINATI**: I dati sono già molto vicini all'essere ordinati!
2. **Pattern crescente**: Partono da 5000 e crescono gradualmente
3. **Piccole variazioni**: Il rumore è relativamente piccolo
4. **Struttura predicibile**: Seguono la curva sinusoidale + rumore

## Trovato 1° algoritmo migliore di quicksort

Quicksort è Pessimo per Dati **quasi ordinati**

I nostri dati sono quasi ordinati (pattern sinusoidale + piccolo rumore). Il quicksort:

1. Non sfrutta il fatto che i dati sono già quasi in ordine
2. Divide sempre anche quando non serve
3. Fa sempre O(n log n) confronti anche su dati quasi ordinati
4. Il pivot spesso cade in posizioni sbagliate

per dati quasi ordinati:

Insertion Sort: O(n) nel caso migliore (quasi ordinati)
Quicksort: Sempre O(n log n), non si adatta

- Implemento un'insertion sort modificato.

  1. Una sola lettura per Elemento

     ```c
     ct_read++; // UNA volta sola
     int key = A[i]; // salva in variabile locale (free)
     ```

  2. Ottimizzazione per dati quasi ordinati

     ```c
      if (A[j] <= key) {
         continue; //  0 spostamenti se già ordinati
     }
     ```

  3. Sposta Blocchi Senza Riletture:

     ```c
      A[j + 1] = A[j]; // usa valore già letto
     ```

Ho ottenuto una riduzione del 76% nelle letture

Confronto Prestazioni:

| Algoritmo       | Letture Medie | Miglioramento |
| --------------- | ------------- | ------------- |
| Quicksort       | 90.190        |               |
| InsertionSOpt   | 37.477        | -76%          |

N test: 100, Min: 374066, Med: 374775.1, Max: 375587

## Ragionamenti successivi specifici sul mio problema, informandomi sul problema del sorting

- Ora che ho raggiunto oquesto risultato ho pensato a ragionare sulla struttura dei dati e come vengono gestiti in particolare cercando di fare meno letture possibili:
Mi sono informato su come potrei minimizzare le letture usando approcci "furbi" per questo problema.
- Ho pensato fin da subito ad un approccio ibrido bit-based
- Esempio: conosco a priori i dati: range, la loro struttura (e questa informazione devo usarla per putnare a fare meglio)
- Nei miei dati range è LIMITATO e NOTO:
  - Min: ~0-500 (nelle parti basse della sinusoide)
  - Max: ~10000 (picco della sinusoide)
  - Range totale: ~10500 valori

- Anche ottimizzato il mio insertion sort caso peggiore O(n^2) letture perchè confronto elementi con quelli precedenti. => quindi non vorrei fare confronti

- Intuzione: posso usare counting sort: **range << n^2**
Quindi invece di confrontare elementi li conto:

  ```c
  // Invece di: "A[j] <= key?" (confronto)
  // Fai: count[valore]++ (conteggio)
  ```

- Quindi ho scritto nuovo algoritmo seguendo questa logica. Iniziando con il setup del problema
  1. setup
  2. creo array contatori **unsigned short**:
     - ogni valore appare max 1000 volte.
     - **unsigned short** = 0 a 65535 (per il range)
     - meno memoria di int (cache performance)

  3. conto occorrenze (Conteggio)

  4. ricostruzione.
     - 1k letture totali
     - non importa se sono ordinati o meno
     - conto occorrenze, per ogni possibile valore quante volte appare scrivilo freq volte, scrittura non lettura.

  5. funziona perchè:
     - Scandisci valori in ordine crescente (0, 1, 2, ...)
     - Per ogni valore, lo scrivi tante volte quanto appare
     - array automaticamente ordinato!

  6. Vantraggi chiave:
     - Letture Costanti: Sempre n letture, mai di più
     - Non Adattivo Necessario: Counting sort non dipende dall'ordine iniziale
     - Cache Friendly: Array di contatori è sequenziale
     - Optimal per i Tuoi Dati: Range limitato + elementi molti

quindi in totale faccio circa: **11k letture**, meglio di prima.

```bash
N test: 100, Min: 11501, Med: 11501.0, Max: 11501
```

## Posso fare meglio

- Punto chiave: evitare le 10501 letture di:

  ```c
  unsigned short frequenza = count[val];
  ```

- il problema è che leggo tutto il range teorico ovvero 10501 letture che mi ammazza letteralmente.

Per ottimizzare ulteriormente l'algoritmo di ordinamento e ridurre il numero di accessi in memoria, ho implementato una versione avanzata del Counting Sort che utilizza una bitmap per registrare i valori distinti. Questo approccio riduce significativamente i letture necessarie durante la fase di scrittura dell'array ordinato.

### Modifiche Implementate

#### Bitmap per Valori Distinti

Invece di scorrere l'intero range (0-10500), una bitmap registra quali valori sono presenti nell'array.

La bitmap è un array di byte dove ogni bit rappresenta la presenza (1) o assenza (0) di un valore nel range.

#### Fase di Conteggio Aggiornata

Per ogni elemento:

- Incrementa il conteggio del valore.
- Aggiorna la bitmap per segnalare la presenza del valore (se non già registrato).
- Ogni accesso agli array (A, count, bitmap) viene contato come lettura.

#### Fase di Scrittura Efficiente

- Scorre la bitmap anziché l'intero range.
- Per ogni bit attivo nella bitmap, legge il conteggio corrispondente e scrive il valore nell'array ordinato.
- I valori sono scritti in ordine crescente grazie alla scansione sequenziale della bitmap.

#### Calcolo Efficiente della Bitmap

- La dimensione della bitmap è calcolata come (range + 7) / 8 per coprire tutto il range con un minimo di byte.
- L'indicizzazione nella bitmap sfrutta divisione e modulo per byte e bit.

#### Perché la bitmap è una soluzione furba

L'uso della bitmap nella versione ottimizzata di counting sort permette di evitare la scansione completa di tutto il range di valori possibili durante la fase di scrittura. Invece di leggere il contatore di ogni valore da 0 a 10500 (anche per valori mai presenti), la bitmap consente di saltare direttamente ai soli valori effettivamente presenti nell'array. Questo riduce drasticamente il numero di letture in memoria, che è l'obiettivo principale della consegna.

**In sintesi:**

- La bitmap è un array di bit, dove ogni bit rappresenta la presenza o assenza di un valore nel range.
- Durante la fase di conteggio, per ogni valore incontrato si aggiorna sia il contatore che il bit corrispondente nella bitmap.
- Durante la fase di scrittura, si scorre solo la bitmap: per ogni bit attivo si legge il contatore e si scrive il valore nell'array ordinato.
- Questo approccio è particolarmente efficace quando il range dei valori è molto più grande del numero di valori effettivamente presenti.

**Risultato:**  
Si ottiene un algoritmo di ordinamento stabile, molto efficiente in termini di letture di memoria, ideale per dati con range limitato e noto ma distribuzione sparsa.

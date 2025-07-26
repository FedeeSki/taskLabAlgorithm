#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

// compilazione: g++ consegna1-loader.c
//

// Il programma carica il file data.txt contenente 100 righe con dati da ordinare in modo crescente
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file

int ct_swap = 0;
int ct_cmp = 0;
int ct_read = 0;

int max_dim = 0;
int ntests = 100;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

bool is_sorted(int *A, int dim) {
    for (int i = 1; i < dim; i++) {
        if (A[i] < A[i-1]) {
            return false;
        }
    }
    return true;
}

// void count_opt_sort(int *A, int p, int r){
//     int n = r - p + 1; //dim sub-array

//     const int min_valore = 0;
//     const int max_valore = 10500;
//     const int range = max_valore + 1;  // 10501 posizioni (0 ,1 , ... , 10500)

//     unsigned short *count = new unsigned short[range](); //0 inizializzato

//     for (int i = p; i <= r; i++){
//         ct_read++;
//         count[A[i]]++; //incremento contatore
//     }

//     int index = p;
//     for (int val = 0; val < range; val++){
//         ct_read++;
//         unsigned short frequenza = count[val];
//         while(frequenza-- > 0){
//             A[index++] = val;
//         }
//     }
// }

void count_opt_sort_bitmap(int *A, int p, int r) {
    const int min_valore = 0;
    const int max_valore = 10500;
    const int range = max_valore - min_valore + 1; // 10501
    const int bitmap_size = (range + 7) / 8; // 1313 byte
    // Calcola il numero di byte necessari per rappresentare 'range' valori con una bitmap.
    // Ogni byte contiene 8 bit, quindi si divide il range per 8.
    // Si aggiunge 7 prima della divisione per arrotondare verso l'alto (così anche se il range non è multiplo di 8, si coprono tutti i valori).
    // Esempio: se range=10501, (10501+7)/8 = 1313 byte, 
    // sufficienti per rappresentare tutti i valori da 0 a 10500
    
    // occorennze
    unsigned short *count = new unsigned short[range](); 
    //valori "effettivamente" presenti
    unsigned char *bitmap = new unsigned char[bitmap_size]();
    int index = p;

    // Fase 1: conteggio e aggiornamento bitmap
    for (int i = p; i <= r; i++) {
        ct_read++; // Lettura A[i]
        int val = A[i];
        if (val < min_valore || val > max_valore) continue;
        int idx = val;

        ct_read++; // Lettura count[idx]
        count[idx]++;

        int byte_idx = idx / 8;
        int bit_idx = idx % 8;
        ct_read++; // Lettura bitmap[byte_idx]
        // Operatore & (AND bit a bit): controlla se il bit in posizione bit_idx è già attivo (presenza del valore)
        if ((bitmap[byte_idx] & (1 << bit_idx)) == 0) {
            // Operatore | (OR bit a bit): attiva il bit in posizione bit_idx per segnare la presenza del valore
            bitmap[byte_idx] |= (1 << bit_idx);
        }
    }

    // Fase 2: scrittura valori ordinati
    for (int byte_idx = 0; byte_idx < bitmap_size; byte_idx++) {
        ct_read++; // Lettura bitmap[byte_idx]
        unsigned char byte = bitmap[byte_idx];
        if (byte == 0) continue;
        for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
            // Operatore & (AND bit a bit): controlla se il bit in posizione bit_idx è attivo (valore presente)
            if (byte & (1 << bit_idx)) {
                int val = byte_idx * 8 + bit_idx;
                if (val < range) {
                    ct_read++; // Lettura count[val]
                    unsigned short freq = count[val];
                    while (freq-- > 0) {
                        A[index++] = val;
                    }
                }
            }
        }
    }

    delete[] count;
    delete[] bitmap;
}



int parse_cmd(int argc, char **argv) {

    /// parsing argomento
    max_dim = 1000;

    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'd')
            ndiv = atoi(argv[i] + 3);
        if (argv[i][1] == 't')
            ntests = atoi(argv[i] + 3);
        if (argv[i][1] == 'v')
            details = 1;
        if (argv[i][1] == 'g') {
            graph = 1;
            ndiv = 1;
            ntests = 1;
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    int i, test;
    int *A;
    int *B; /// buffer per visualizzazione algoritmo

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];

    n = max_dim;

    /*
        srand((unsigned)time(NULL));
        //creazione file input: NON USARE PIU' --> il file data.txt ufficiale è stato allegato, per permettere confronti equivalenti
        FILE *f = fopen("data.txt", "w+");
        int size=100;
        for (int j = 0; j < size; j++) {
            for (int i = 0; i < n; i++) {
              int v = 0;             
              v=(int)(100000*exp(-(0.0+i-n/2)*(0.0+i-n/2)/n/n*64));
              v=(int)(5000*(1+sin(3.1415*(i/(n/2.0)))));
              v+=rand()%(1+(int)(n*pow(((i+0.0)/n),2)));
              fprintf(f, "%d,", v);
          }
          fprintf(f, "\n");
      }
      fclose(f);
    */

    ifstream input_data;
    input_data.open("data.txt");

    int read_min = -1;
    int read_max = -1;
    long read_avg = 0;

    //// lancio ntests volte per coprire diversi casi di input random
    for (test = 0; test < ntests; test++) {

        /// inizializzazione array: numeri random con range dimensione array
        for (i = 0; i < n; i++) {
            char comma;
            input_data >> A[i];
            input_data >> comma;
        }

        if (details) {
            printf("caricato array di dimensione %d\n", n);
            print_array(A, n);
        }

        ct_swap = 0;
        ct_cmp = 0;
        ct_read = 0;

        //count_opt_sort(A, 0, n-1);
        count_opt_sort_bitmap(A, 0, n-1);

        if (!is_sorted(A, n)) {
            printf("ERRORE: Array non ordinato correttamente al test %d!\n", test);
            if (details) {
                printf("Array risultante:\n");
                print_array(A, n);
            }
            return 1; 
        } else if (details) {
            printf("✓ Array ordinato correttamente\n");
        }

        if (details) {
            printf("Output:\n");
            print_array(A, n);
        }

        /// statistiche
        read_avg += ct_read;
        if (read_min < 0 || read_min > ct_read)
            read_min = ct_read;
        if (read_max < 0 || read_max < ct_read)
            read_max = ct_read;
        printf("Test %d %d\n", test, ct_read);
    }

    printf("N test: %d, Min: %d, Med: %.1f, Max: %d\n",
           ntests,
           read_min, (0.0 + read_avg) / ntests, read_max);

    delete[] A;

    return 0;
}

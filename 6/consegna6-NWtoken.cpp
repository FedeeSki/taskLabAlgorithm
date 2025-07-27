#include <iostream>
#include <cstdio>   

// --- FUNZIONI DI UTILITÀ PER I CARATTERI (tipo quelli di <cctype>) ---

bool my_isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

bool my_isalpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool my_isdigit(char c) {
    return c >= '0' && c <= '9';
}

bool my_isalnum(char c) {
    return my_isalpha(c) || my_isdigit(c);
}


// STRING


class String {
private:
    char* data;
    int len;
    int cap; // Capacità, per rendere push_back efficiente

   
    int custom_strlen(const char* s) const {
        int length = 0;
        while (s[length] != '\0') {
            length++;
        }
        return length;
    }

    void custom_strcpy(char* dest, const char* src) const {
        int i = 0;
        while (src[i] != '\0') {
            dest[i] = src[i];
            i++;
        }
        dest[i] = '\0';
    }

    int custom_strcmp(const char* s1, const char* s2) const {
        int i = 0;
        while (s1[i] != '\0' && s2[i] != '\0') {
            if (s1[i] != s2[i]) {
                return s1[i] - s2[i];
            }
            i++;
        }
        return s1[i] - s2[i];
    }

public:
    String() : data(new char[1]), len(0), cap(1) {
        data[0] = '\0';
    }

    // Costruttore da const char*
    String(const char* s) {
        len = custom_strlen(s);
        cap = len;
        data = new char[cap + 1];
        custom_strcpy(data, s);
    }

    // Costruttore di copia
    String(const String& other) : len(other.len), cap(other.cap) {
        data = new char[cap + 1];
        custom_strcpy(data, other.data);
    }

    // Operatore di assegnazione
    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            len = other.len;
            cap = other.cap;
            data = new char[cap + 1];
            custom_strcpy(data, other.data);
        }
        return *this;
    }

    // Distruttore
    ~String() {
        delete[] data;
    }

    // Aggiunge un carattere alla fine della stringa

    void push_back(char ch) {
    // Se la lunghezza ha raggiunto la capacità attuale, è necessario riallocare più memoria
    if (len == cap) {

        // Se cap == 0, inizializzarla a 1; altrimenti *2 (esponenziale crescita)
        cap = (cap == 0) ? 1 : cap * 2;

        // Alloca un nuovo array con la nuova capacità +1 per il terminatore null
        char* newData = new char[cap + 1];
        custom_strcpy(newData, data); // copy data in new dda
         
        delete[] data; // delete mem precedente
        
        data = newData; // update pointer ai dati new
    }

    // add il carattere alla fine della stringa
    data[len] = ch;

    // Incrementa la lunghezza della stringa
    len++;

    data[len] = '\0'; // Aggiunge il terminatore null 
}


    const char* c_str() const { return data; }
    int size() const { return len; }

    bool operator==(const String& other) const {
        if (len != other.len) return false;
        return custom_strcmp(data, other.c_str()) == 0;
    }
};


// VECTOR 

template <typename T>
class Vector {
private:
    T* data;
    int current_size;
    int current_capacity;

public:
    Vector(int initial_capacity = 10) 
        : data(new T[initial_capacity]), current_size(0), current_capacity(initial_capacity) {}

    ~Vector() {
        delete[] data;
    }
    
    // Costruttore di copia
    Vector(const Vector& other) 
        : current_size(other.current_size), current_capacity(other.current_capacity) {
        data = new T[current_capacity];
        for (int i = 0; i < current_size; ++i) {
            data[i] = other.data[i];
        }
    }

    // Operatore di assegnazione
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            delete[] data;
            current_size = other.current_size;
            current_capacity = other.current_capacity;
            data = new T[current_capacity];
            for (int i = 0; i < current_size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }


    void push_back(const T& value) {
        //logica simil alla stringa
        if (current_size == current_capacity) {
            int new_capacity = (current_capacity == 0) ? 1 : current_capacity * 2;
            T* new_data = new T[new_capacity];
            for (int i = 0; i < current_size; ++i) {
                new_data[i] = data[i];
            }
            delete[] data;
            data = new_data;
            current_capacity = new_capacity;
        }
        data[current_size] = value;
        current_size++;
    }

    T& operator[](int index) {
        return data[index];
    }

    const T& operator[](int index) const {
        return data[index];
    }

    int size() const {
        return current_size;
    }
};


// --- FASE 1: TOKENIZZAZIONE ---

enum TokenType {
    KEYWORD, IDENTIFIER, NUMBER, OPERATOR, SYMBOL
};

// Funzione helper per verificare se una stringa è una keyword C++
// Questo è cruciale per il sistema di punteggio.
bool is_keyword(const String& s) {
    const char* keywords[] = {
        "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
        "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
        "class", "compl", "concept", "const", "consteval", "constexpr", "constinit",
        "const_cast", "continue", "co_await", "co_return", "co_yield", "decltype",
        "default", "delete", "do", "double", "dynamic_cast", "else", "enum",
        "explicit", "export", "extern", "false", "float", "for", "friend", "goto",
        "if", "inline", "int", "long", "mutable", "namespace", "new", "noexcept",
        "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
        "protected", "public", "register", "reinterpret_cast", "requires", "return",
        "short", "signed", "sizeof", "static", "static_assert", "static_cast",
        "struct", "switch", "template", "this", "thread_local", "throw", "true",
        "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
        "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
    };
    int num_keywords = sizeof(keywords) / sizeof(keywords[0]); //keyw in array
    for (int i = 0; i < num_keywords; ++i) {
        if (s == String(keywords[i])) {
            return true; // è una keyword
        }
    }
    return false;
}

struct Token {
    TokenType type;
    String text; 
};

Vector<Token> tokenize(const char* filename) {
    Vector<Token> tokens;
    FILE* file = fopen(filename, "r");
    if (!file) {
        std::cout << "Errore: Impossibile aprire il file " << filename << std::endl;
        return tokens;
    }

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (my_isspace(c)) { // ignoro white space , tab , nline
            continue;
        }
        // Identificatori e parole chiave (lettara o _ )
        if (my_isalpha(c) || c == '_') {
            String current_text;
            current_text.push_back(c);

            while ((c = fgetc(file)) != EOF && (my_isalnum(c) || c == '_')) { //continua finche lettere cifre o _
                current_text.push_back(c);
            }
            //se abbiamo letto un carattere in più, lo ignoriamo

            TokenType type = is_keyword(current_text) ? KEYWORD : IDENTIFIER; //keyword
            Token t = {type, current_text};
            tokens.push_back(t);

            if (c == EOF) break; // Se è stato letto un carattere in più (non valido), verrà processato nel prossimo ciclo
        }
        
        else if (my_isdigit(c)) {  // num interi
            String current_text;
            current_text.push_back(c);

            while ((c = fgetc(file)) != EOF && my_isdigit(c)) { // continua a leggere finche trova cifre
                current_text.push_back(c);
            }
            Token t = {NUMBER, current_text};
            tokens.push_back(t);

            if (c == EOF) break;
        }
        
        else { //simboli e qualsiasi altro carattere
            String text;
            text.push_back(c);

            TokenType type;   // strutturale o operatore
            const char* s = text.c_str(); 
            if (s[0] == '{' || s[0] == '}' || s[0] == '(' || s[0] == ')' || s[0] == '[' || s[0] == ']' || s[0] == ';') {
                type = SYMBOL;
            } else {
                type = OPERATOR;
            }
            Token t = {type, text};
            tokens.push_back(t);
        }
    }
    fclose(file); 
    return tokens; //vector token estratti
}

// --- FASE 2: SISTEMA DI PUNTEGGIO ---

// Valori di punteggio per l'allineamento tra token (const per facile scoring)
const int MATCH_STRUCTURAL_SCORE = 10; // { } ( ) ;
const int MATCH_KEYWORD_SCORE = 5;     // if, for, while...
const int MATCH_GENERIC_SCORE = 2;     // Nomi di variabili, numeri...
const int MISMATCH_PENALTY = -2; // token diversi

//Viene usata per inizializzare la matrice e durante il calcolo dei 
// punteggi di inserzione/cancellazione.
const int GAP_PENALTY = -3;  

int score(const Token& a, const Token& b) {
    if (a.text == b.text) { // identici token
        if (a.type == SYMBOL) {
            return MATCH_STRUCTURAL_SCORE;
        }
        if (a.type == KEYWORD) {
            return MATCH_KEYWORD_SCORE;
        }
        return MATCH_GENERIC_SCORE; // per IDENTIFIER, NUMBER, OPERATOR, ecc.
    }
    return MISMATCH_PENALTY; // penalita' fissa
}


// --- FASE 3: ALGORITMO DI NEEDLEMAN-WUNSCH ---

// trovare il MAX di tre interi
int max3(int a, int b, int c) { 
    int max_val = a;
    if (b > max_val) max_val = b;
    if (c > max_val) max_val = c;
    return max_val;
}

void needlemanWunsch(const Vector<Token>& seq1, const Vector<Token>& seq2) {
    int n = seq1.size();
    int m = seq2.size();

    // 1. Creazione e inizializzazione della matrice dei punteggi
    
    Vector<Vector<int>> matrix;
    for (int i = 0; i <= n; ++i) {
        Vector<int> row;
        for (int j = 0; j <= m; ++j) {
            row.push_back(0); // Riempiamo la riga con valori iniziali
        }
        matrix.push_back(row); // Aggiungiamo la riga completa alla matrice
    }

    for (int i = 0; i <= n; ++i) {
        matrix[i][0] = i * GAP_PENALTY;
    }
    for (int j = 0; j <= m; ++j) {
        matrix[0][j] = j * GAP_PENALTY;
    }

    // 2. Riempimento della matrice
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            int match_score = matrix[i - 1][j - 1] + score(seq1[i - 1], seq2[j - 1]);
            int del_score = matrix[i - 1][j] + GAP_PENALTY;
            int ins_score = matrix[i][j - 1] + GAP_PENALTY;
            matrix[i][j] = max3(match_score, del_score, ins_score);
        }
    }

    // 3. Backtracking per stampare l'allineamento
    std::cout << "\n--- Risultato dell'Allineamento ---" << std::endl;
    int i = n;
    int j = m;

    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && matrix[i][j] == matrix[i - 1][j - 1] + score(seq1[i - 1], seq2[j - 1])) {
            // Match o Mismatch
            if (score(seq1[i - 1], seq2[j - 1]) > 0) { // Match
                std::cout << "MATCH: " << seq1[i - 1].text.c_str() << std::endl;
            } else { // Mismatch
                std::cout << "CHANGE: " << seq1[i - 1].text.c_str() << " -> " << seq2[j - 1].text.c_str() << std::endl;
            }
            i--;
            j--;
        } else if (i > 0 && matrix[i][j] == matrix[i - 1][j] + GAP_PENALTY) {
            // Cancellazione
            std::cout << "DELETE: " << seq1[i - 1].text.c_str() << std::endl;
            i--;
        } else {
            // Inserzione
            std::cout << "INSERT: " << seq2[j - 1].text.c_str() << std::endl;
            j--;
        }
    }
}


// --- FASE 4: PROGRAMMA PRINCIPALE ---

int main(int argc, char* argv[]) {
    const char* file1_path = "file1.cpp";
    const char* file2_path = "file2.cpp";

    std::cout << "--- Confronto tra " << file1_path << " e " << file2_path << " ---" << std::endl;

    // 1. Tokenizza entrambi i file
    std::cout << "\n[FASE 1: Tokenizzazione]" << std::endl;
    Vector<Token> tokens1 = tokenize(file1_path);
    Vector<Token> tokens2 = tokenize(file2_path);

    // 2. Esegui l'algoritmo di allineamento
    std::cout << "\n[FASE 2: Allineamento]" << std::endl;
    needlemanWunsch(tokens1, tokens2);

    // 3. Libera la memoria
    std::cout << "\n[FASE 3: Pulizia Memoria]" << std::endl;
    std::cout << "Memoria gestita automaticamente dai distruttori di Vector e String." << std::endl;

    std::cout << "\n--- Confronto terminato ---" << std::endl;

    return 0;
}
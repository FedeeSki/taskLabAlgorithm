#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <cmath>  // per abs()

// Struttura per un nodo dell'albero binario
struct T_Node {
    int data;
    T_Node* left;
    T_Node* right;
    
    // Costruttore
    T_Node(int value) : data(value), left(nullptr), right(nullptr) {}
};

// Classe per gestire l'albero binario
class B_Tree {
private:
    T_Node* root;
    
    // Funzioni helper private
    T_Node* createNodeFromInput();
    T_Node* createRandomNode(int depth, int maxDepth, int minValue, int maxValue);
    void deleteTree(T_Node* node);
    void printInOrder(T_Node* node);
    void printTreeStructure(T_Node* node, std::string prefix = "", bool isLast = true);
    int getTreeHeight(T_Node* node);
    int countNodes(T_Node* node);
    void writeDotHelper(T_Node* node, std::ofstream& file);
    void writeDotNodesHelper(T_Node* node, std::ofstream& file);
    void writeDotEdgesHelper(T_Node* node, std::ofstream& file);
    //Funzioni consegna.
    void flipTreeHelper(T_Node* node);
    void printNodeDepthsHelper(T_Node* node, int currentDepth);
    int isBalancedHelper(T_Node* node);
    
public:
    // Costruttore e Distruttore
    B_Tree();
    ~B_Tree();
    
    // Funzione per leggere l'albero da input
    void readTreeFromInput();
    
    // Funzione per generare albero random
    void generateRandomTree(int maxDepth, int minValue, int maxValue);
    
    // Funzione per visualizzare l'albero
    void display();
    
    // Funzione per generare file DOT
    void generateDotFile(const std::string& filename = "tree.dot");
    
    // Funzione per flip dell'albero
    void flipTree();
    
    // Funzione per calcolare e mostrare profondità nodi
    void printNodeDepths();
    
    // Funzione per verificare se l'albero è bilanciato
    bool isBalanced();


};

// Implementazione delle funzioni

// Costruttore
B_Tree::B_Tree() {
    root = nullptr;
    std::srand(std::time(0)); // Inizializza il generatore random
}

// Distruttore
B_Tree::~B_Tree() {
    deleteTree(root);
}

// Funzione helper per eliminare l'albero
void B_Tree::deleteTree(T_Node* node) {
    if (node != nullptr) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

// Funzione helper per creare un nodo da input
T_Node* B_Tree::createNodeFromInput() {
    int value;
    std::cout << "Inserisci valore del nodo (-1 per nodo vuoto): ";
    std::cin >> value;
    
    if (value == -1) {
        return nullptr;
    }
    
    T_Node* node = new T_Node(value);
    
    std::cout << "Inserisci figlio sinistro di " << value << ":" << std::endl;
    node->left = createNodeFromInput();
    
    std::cout << "Inserisci figlio destro di " << value << ":" << std::endl;
    node->right = createNodeFromInput();
    
    return node;
}

// Funzione per leggere l'albero da input
void B_Tree::readTreeFromInput() {
    std::cout << "Costruzione dell'albero binario:" << std::endl;
    std::cout << "Inserisci la radice:" << std::endl;
    root = createNodeFromInput();
}

// Funzione helper per creare un nodo random
T_Node* B_Tree::createRandomNode(int depth, int maxDepth, int minValue, int maxValue) {
    // Se abbiamo raggiunto la profondità massima, ritorna nullptr
    if (depth >= maxDepth) {
        return nullptr;
    }
    
    // Probabilità del 70% di creare un nodo (per avere un albero non troppo pieno)
    if (std::rand() % 100 < 30) {
        return nullptr;
    }
    
    // Genera un valore random nel range specificato
    int value = minValue + std::rand() % (maxValue - minValue + 1);
    T_Node* node = new T_Node(value);
    
    // Ricorsivamente crea i figli
    node->left = createRandomNode(depth + 1, maxDepth, minValue, maxValue);
    node->right = createRandomNode(depth + 1, maxDepth, minValue, maxValue);
    
    return node;
}

// Funzione per generare albero random
void B_Tree::generateRandomTree(int maxDepth, int minValue, int maxValue) {
    // Pulisce l'albero esistente
    deleteTree(root);
    
    std::cout << "Generazione albero random..." << std::endl;
    std::cout << "Profondità massima: " << maxDepth << std::endl;
    std::cout << "Range valori: [" << minValue << ", " << maxValue << "]" << std::endl;
    
    // Crea sempre una radice
    int rootValue = minValue + std::rand() % (maxValue - minValue + 1);
    root = new T_Node(rootValue);
    
    // Genera i sottoalberi
    root->left = createRandomNode(1, maxDepth, minValue, maxValue);
    root->right = createRandomNode(1, maxDepth, minValue, maxValue);
}

// Funzione helper per stampa in-order con decorazione
void B_Tree::printInOrder(T_Node* node) {
    if (node != nullptr) {
        printInOrder(node->left);
        std::cout << "[" << node->data << "] ";
        printInOrder(node->right);
    }
}

// Funzione per stampare la struttura dell'albero in modo grafico
void B_Tree::printTreeStructure(T_Node* node, std::string prefix, bool isLast) {
    if (node != nullptr) {
        std::cout << prefix;
        std::cout << (isLast ? "└── " : "├── ");
        std::cout << node->data << std::endl;
        
        // Conta i figli per determinare quale è l'ultimo
        bool hasLeft = (node->left != nullptr);
        bool hasRight = (node->right != nullptr);
        
        if (hasLeft || hasRight) {
            if (hasLeft) {
                printTreeStructure(node->left, prefix + (isLast ? "    " : "│   "), !hasRight);
            }
            if (hasRight) {
                printTreeStructure(node->right, prefix + (isLast ? "    " : "│   "), true);
            }
        }
    }
}

// Funzione per calcolare l'altezza dell'albero
int B_Tree::getTreeHeight(T_Node* node) {
    if (node == nullptr) {
        return 0;
    }
    
    int leftHeight = getTreeHeight(node->left);
    int rightHeight = getTreeHeight(node->right);
    
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

// Funzione per contare i nodi
int B_Tree::countNodes(T_Node* node) {
    if (node == nullptr) {
        return 0;
    }
    
    return 1 + countNodes(node->left) + countNodes(node->right);
}

// Funzione per visualizzare l'albero in modo carino
void B_Tree::display() {
    if (root == nullptr) {
        std::cout << "┌─────────────────────────────────┐" << std::endl;
        std::cout << "│        L'ALBERO È VUOTO         │" << std::endl;
        std::cout << "└─────────────────────────────────┘" << std::endl;
        return;
    }
    
    int height = getTreeHeight(root);
    int nodeCount = countNodes(root);
    
    std::cout << "\n╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "                    ALBERO BINARIO                    " << std::endl;
    std::cout << "   Altezza: " << height << "                             " << std::endl;
    std::cout << "   Numero nodi: " << nodeCount << "                      " << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\n🌳 STRUTTURA DELL'ALBERO:" << std::endl;
    std::cout << "┌─ Radice" << std::endl;
    printTreeStructure(root);
    
    std::cout << "\n TRAVERSAL IN-ORDER (ordinato):" << std::endl;
    std::cout << "┌─ Sequenza: ";
    printInOrder(root);
    std::cout << std::endl;
    
    std::cout << "\n" << std::string(50, '-') << std::endl;
}

// Funzione helper per scrivere solo i nodi nel file DOT
void B_Tree::writeDotNodesHelper(T_Node* node, std::ofstream& file) {
    if (node == nullptr) {
        return;
    }
    
    // Usa l'indirizzo del nodo come ID univoco e mostra il valore come label
    file << "    node" << node << " [label=\"" << node->data << "\", shape=circle, style=filled, fillcolor=lightblue, fontsize=14, fontweight=bold];" << std::endl;
    
    // Visita ricorsivamente i figli
    writeDotNodesHelper(node->left, file);
    writeDotNodesHelper(node->right, file);
}

// Funzione helper per scrivere solo gli archi nel file DOT
void B_Tree::writeDotEdgesHelper(T_Node* node, std::ofstream& file) {
    if (node == nullptr) {
        return;
    }
    
    // Se ha figlio sinistro, crea l'arco usando gli ID univoci
    if (node->left != nullptr) {
        file << "    node" << node << " -> node" << node->left << " [label=\"L\", color=red, fontcolor=red, fontsize=10];" << std::endl;
    }
    
    // Se ha figlio destro, crea l'arco usando gli ID univoci
    if (node->right != nullptr) {
        file << "    node" << node << " -> node" << node->right << " [label=\"R\", color=blue, fontcolor=blue, fontsize=10];" << std::endl;
    }
    
    // Visita ricorsivamente i figli
    writeDotEdgesHelper(node->left, file);
    writeDotEdgesHelper(node->right, file);
}

// Funzione helper per scrivere i nodi e gli archi nel file DOT (mantenuta per compatibilità)
void B_Tree::writeDotHelper(T_Node* node, std::ofstream& file) {
    // Non più utilizzata, ma mantenuta per evitare errori
}

// Funzione per generare il file DOT
void B_Tree::generateDotFile(const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cout << "❌ Errore nell'apertura del file: " << filename << std::endl;
        return;
    }
    
    // Header del file DOT
    file << "digraph BinaryTree {" << std::endl;
    file << "    // Configurazione generale del grafo" << std::endl;
    file << "    rankdir=TB;" << std::endl;  // Top-Bottom layout
    file << "    node [fontname=\"Arial\"];" << std::endl;
    file << "    edge [fontname=\"Arial\"];" << std::endl;
    file << "    bgcolor=white;" << std::endl;
    file << "    " << std::endl;
    file << "    // Titolo del grafo" << std::endl;
    file << "    label=\"Albero Binario\\nGenerato automaticamente\";" << std::endl;
    file << "    labelloc=top;" << std::endl;
    file << "    fontsize=16;" << std::endl;
    file << "    fontweight=bold;" << std::endl;
    file << "    " << std::endl;
    
    if (root == nullptr) {
        file << "    // Albero vuoto" << std::endl;
        file << "    empty [label=\"Albero Vuoto\", shape=box, style=filled, fillcolor=lightgray];" << std::endl;
    } else {
        file << "    // Nodi dell'albero" << std::endl;
        writeDotNodesHelper(root, file);
        file << "    " << std::endl;
        file << "    // Archi dell'albero" << std::endl;
        writeDotEdgesHelper(root, file);
    }
    
    file << "}" << std::endl;
    file.close();
    
    std::cout << "\n📁 File DOT generato: " << filename << std::endl;
    std::cout << "💡 Per visualizzare l'albero graficamente, usa:" << std::endl;
    std::cout << "   dot -Tpng " << filename << " -o albero.png" << std::endl;
    std::cout << "   (richiede Graphviz installato)" << std::endl;
}

//FUNZIONI PER CONSEGNA


// Funzione helper per flip dell'albero 1° Consegna
void B_Tree::flipTreeHelper(T_Node* node) {
    if (node == nullptr) {
        return;
    }
    
    // Scambia i figli sinistro e destro
    T_Node* temp = node->left;
    node->left = node->right;
    node->right = temp;
    
    // Ricorsivamente applica il flip ai sottoalberi
    flipTreeHelper(node->left);
    flipTreeHelper(node->right);
}

// Funzione pubblica per flip dell'albero
void B_Tree::flipTree() {
    if (root == nullptr) {
        std::cout << "❌ Impossibile fare flip: l'albero è vuoto!" << std::endl;
        return;
    }
    
    std::cout << "🔄 Eseguendo flip dell'albero..." << std::endl;
    flipTreeHelper(root);
    std::cout << "✅ Flip completato! Ogni sottoalbero sinistro è diventato destro e viceversa." << std::endl;
}

// Funzione helper per calcolare e stampare la profondità di ogni nodo 2° Consegna
void B_Tree::printNodeDepthsHelper(T_Node* node, int currentDepth) {
    if (node == nullptr) {
        return;
    }
    
    // Stampa il nodo corrente con la sua profondità
    std::cout << "    Nodo [" << node->data << "] -> Profondità: " << currentDepth << std::endl;
    
    // Ricorsivamente visita i sottoalberi incrementando la profondità
    printNodeDepthsHelper(node->left, currentDepth + 1);
    printNodeDepthsHelper(node->right, currentDepth + 1);
}

// Funzione pubblica per calcolare e mostrare la profondità di tutti i nodi
void B_Tree::printNodeDepths() {
    if (root == nullptr) {
        std::cout << "❌ Impossibile calcolare profondità: l'albero è vuoto!" << std::endl;
        return;
    }
    
    std::cout << "📏 Calcolando profondità di ogni nodo..." << std::endl;
    std::cout << "┌─ Profondità nodi (radice = 0):" << std::endl;
    printNodeDepthsHelper(root, 0);
    std::cout << "✅ Calcolo profondità completato!" << std::endl;
}

// Funzione helper per verificare se l'albero è bilanciato O(n)
int B_Tree::isBalancedHelper(T_Node* node) {
    // Caso base: albero vuoto è bilanciato con altezza 0
    if (node == nullptr) {
        return 0;
    }
    
    // Verifica ricorsivamente sottoalbero sinistro
    int leftHeight = isBalancedHelper(node->left);
    if (leftHeight == -1) {  // Sottoalbero sinistro non bilanciato
        return -1;
    }
    
    // Verifica ricorsivamente sottoalbero destro
    int rightHeight = isBalancedHelper(node->right);
    if (rightHeight == -1) {  // Sottoalbero destro non bilanciato
        return -1;
    }
    
    // Verifica se il nodo corrente è bilanciato
    int heightDiff = abs(leftHeight - rightHeight);
    if (heightDiff > 1) {
        return -1;  // Non bilanciato
    }
    
    // Ritorna l'altezza del sottoalbero corrente
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

// Funzione pubblica per verificare se l'albero è bilanciato
bool B_Tree::isBalanced() {
    if (root == nullptr) {
        std::cout << "🌿 Albero vuoto: considerato bilanciato per definizione." << std::endl;
        return true;
    }
    
    std::cout << "⚖️  Verificando se l'albero è bilanciato..." << std::endl;
    
    int result = isBalancedHelper(root);
    bool balanced = (result != -1);
    
    if (balanced) {
        std::cout << "✅ L'albero È BILANCIATO! (altezza: " << result << ")" << std::endl;
        std::cout << "   Per ogni nodo, la differenza di altezza tra sottoalberi ≤ 1" << std::endl;
    } else {
        std::cout << "❌ L'albero NON è bilanciato!" << std::endl;
        std::cout << "   Esiste almeno un nodo con differenza di altezza > 1" << std::endl;
    }
    
    return balanced;
}

// Funzione main
int main() {
    B_Tree tree;
    int choice;
    
    std::cout << "╔══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               GENERATORE ALBERO BINARIO              ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║  1. 📝 Inserimento manuale                           ║" << std::endl;
    std::cout << "║  2. 🎲 Generazione casuale                           ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "🔸 Scelta: ";
    std::cin >> choice;
    
    if (choice == 1) {
        std::cout << "\n📝 MODALITÀ INSERIMENTO MANUALE" << std::endl;
        std::cout << "💡 Suggerimento: inserisci -1 per indicare un nodo vuoto\n" << std::endl;
        tree.readTreeFromInput();
    } else if (choice == 2) {
        int maxDepth, minValue, maxValue;
        
        std::cout << "\n🎲 MODALITÀ GENERAZIONE CASUALE" << std::endl;
        std::cout << "🔸 Inserisci profondità massima dell'albero: ";
        std::cin >> maxDepth;
        
        std::cout << "🔸 Inserisci valore minimo: ";
        std::cin >> minValue;
        
        std::cout << "🔸 Inserisci valore massimo: ";
        std::cin >> maxValue;
        
        std::cout << "\n⚙️  Generazione in corso..." << std::endl;
        tree.generateRandomTree(maxDepth, minValue, maxValue);
        std::cout << "✅ Albero generato con successo!" << std::endl;
    } else {
        std::cout << "❌ Scelta non valida!" << std::endl;
        return 1;
    }
    
    // Visualizza l'albero
    tree.display();
    
    // Loop principale per le operazioni
    while (true) {
        std::cout << "\n╔══════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                   OPERAZIONI ALBERO                  ║" << std::endl;
        std::cout << "╠══════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║  1. 🔄 Flip albero (scambia sottoalberi sx/dx)       ║" << std::endl;
        std::cout << "║  2. 📏 Calcola profondità nodi                       ║" << std::endl;
        std::cout << "║  3. ⚖️  Verifica se albero è bilanciato              ║" << std::endl;
        std::cout << "║  4. 📊 Mostra albero                                 ║" << std::endl;
        std::cout << "║  5. 🎨 Genera file DOT                               ║" << std::endl;
        std::cout << "║  q. ❌ Esci                                          ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════╝" << std::endl;
        std::cout << "🔸 Scelta: ";
        
        char operation;
        std::cin >> operation;
        
        if (operation == 'q' || operation == 'Q') {
            std::cout << "👋 cya!" << std::endl;
            break;
        }
        
        switch (operation) {
            case '1':
                std::cout << "\n📊 ALBERO PRIMA DEL FLIP:" << std::endl;
                tree.display();
                
                tree.flipTree();
                
                std::cout << "\n📊 ALBERO DOPO IL FLIP:" << std::endl;
                tree.display();
                break;
                
            case '2':
                std::cout << "\n📏 PROFONDITÀ NODI:" << std::endl;
                tree.printNodeDepths();
                break;
                
            case '3':
                std::cout << "\n⚖️  VERIFICA BILANCIAMENTO:" << std::endl;
                tree.isBalanced();
                break;
                
            case '4':
                std::cout << "\n📊 ALBERO CORRENTE:" << std::endl;
                tree.display();
                break;
                
            case '5': {
                std::cout << "\n🎨 Generazione file DOT..." << std::endl;
                tree.generateDotFile("albero.dot");
                break;
            }
            
            default:
                std::cout << "❌ Scelta non valida! Usa 1, 2, 3, 4, 5 o 'q' per uscire." << std::endl;
                break;
        }
    }
    
    return 0;
}
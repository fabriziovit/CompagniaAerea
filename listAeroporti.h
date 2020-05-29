//
// Created by Fabrizio on 29/05/2020.
//
struct nodo{
    int indexDB;
    char codiceAeroporto[10];
    char citta[100];
    int inserito;
    struct nodo* next;
};

typedef struct nodo* aeroporto;

aeroporto initLista(int indexDB, char codice[10], char citta[100], int inserito);
void inserisciCoda(aeroporto *L, int indexDB, char codice[10], char citta[100], int inserito);
void printList(aeroporto L);
int trovaArray(aeroporto L, char codice[10]);
char *trovaCodice(aeroporto L, int indexDB);


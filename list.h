//
// Created by gianl on 28/05/2020.
//

#ifndef COMPAGNIAAEREA_LIST_H
#define COMPAGNIAAEREA_LIST_H

#endif //COMPAGNIAAEREA_LIST_H

struct Nodo{
    char partenza[100];
    char destinazione[100];
    char utente[100];
    struct Nodo* next;
};

typedef struct Nodo* Prenotazioni;

Prenotazioni InizializzaL(char partenza[100], char destinazione[100], char utente[100]);
void StampaL(Prenotazioni L);
void InserisciL(Prenotazioni *L, char partenza[100],  char destinazione[100], char utente[100]);
void EliminaElem(Prenotazioni L, char partenza[100], char destinazione[100], char utente[100]);

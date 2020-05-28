//
// Created by gianl on 28/05/2020.
//

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "sqlite3.h"

Prenotazioni InizializzaL(char partenza[100], char destinazione[100], char utente[100]){
    Prenotazioni L = (Prenotazioni)malloc(sizeof(struct Nodo));
    strcpy(L->partenza, partenza);
    strcpy(L->destinazione, destinazione);
    strcpy(L->utente, utente);
    L->next = NULL;
    return L;
}

void StampaL(Prenotazioni L){
    if (L != NULL) {
        printf("Utente: %s\n", L->utente);
        printf("Citta' di Partenza: %s\n", L->partenza);
        printf("Citta' di Destinazione: %s\n", L->destinazione);
        printf("\n");
        StampaL(L->next);
    }
}

void InserisciL(Prenotazioni *L, char partenza[100], char destinazione[100], char utente[100]){
    Prenotazioni ultimo;
    Prenotazioni temp;
    temp = InizializzaL(partenza, destinazione, utente);
    if (*L == NULL) {
        *L = temp;
    } else {
        ultimo = *L;
        while (ultimo->next != NULL)
            ultimo = ultimo->next;
        ultimo->next = temp;
    }
}

void EliminaElem(Prenotazioni L, char partenza[100], char destinazione[100], char utente[100]){
    struct Nodo *tmp = L->next;
    struct Nodo *prev;

    if(tmp != NULL && (strcmp(tmp->partenza, partenza) == 0)){
        L->next = tmp->next;
        free(tmp);
        printf("ELIMINATO");
    }
    while(tmp != NULL && (strcmp(tmp->partenza, partenza) != 0)){
        prev = tmp;
        tmp = tmp->next;
    }
    if(tmp == NULL)
        printf("ERROR");
    prev->next = tmp->next;
    free(tmp);
}
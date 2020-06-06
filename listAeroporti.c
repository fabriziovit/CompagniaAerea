//
// Created by Fabrizio on 29/05/2020.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "listAeroporti.h"

void printList(aeroporto L) {
    if (L != NULL) {
        printf("Codice Aeroporto: %s\n", L->codiceAeroporto);
        printf("Citta` in cui si trova l'aeroporto: %s\n", L->citta);
        printf("\n");
        printList(L->next);
    }
}

aeroporto initLista(int indexDB, char codice[10], char citta[100], int inserito){
    aeroporto L = (aeroporto)malloc(sizeof(struct nodo));
    strcpy(L->citta, citta);
    strcpy(L->codiceAeroporto, codice);
    L->indexDB= indexDB;
    L->inserito = inserito;
    L->next = NULL;
    return L;
}

void inserisciCoda(aeroporto *l, int indexDB, char codice[10], char citta[100] ,int inserito){
    aeroporto ultimo;
    aeroporto temp;
    temp = initLista(indexDB, codice,  citta, inserito);
    if (*l==NULL) {
        *l = temp;
    } else {
        ultimo = *l;
        while (ultimo->next != NULL)
            ultimo = ultimo->next;
        ultimo->next = temp;
    }
}

int trovaArray(aeroporto L, char codice[10]){
    while(L){
        if(strcmp(L->codiceAeroporto,codice) == 0)
            return L->indexDB;
        else
            L=L->next;
    }
    return -1;
}

char *trovaCodice(aeroporto L, int indexDB){
    while(L){
        if(L->indexDB == indexDB)
            return L->codiceAeroporto;
        else
            L=L->next;
    }
    return NULL;
}

char *trovaCitta(aeroporto L, char codice[10]){
    while(L){
        if(strcmp(L->codiceAeroporto,codice) == 0)
            return L->citta;
        else
            L=L->next;
    }
    return NULL;
}

char *trovaCodiceDaCitta(aeroporto L, char citta[100]){
    while(L){
        if(strcmp(L->citta, citta) == 0)
            return L->codiceAeroporto;
        else
            L=L->next;
    }
    return NULL;
}
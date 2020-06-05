//
// Created by gianl on 02/06/2020.
//

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>


typedef struct Nodo_C{
    int dbindex;
    struct Nodo_C *next;
}Nodo_C;

typedef struct Coda{
    Nodo_C *tail;
    Nodo_C *head;
}Coda;


Nodo_C *CreaNodo_C(int dbindex);
bool CodaVuota(Coda *Q);
void CreaCoda(Coda *Q);
void Inserisci_C(Coda *Q, int dbindex);
void Eliminazione_C(Coda *Q, int dbindex);
void EliminaNodo_C(Coda *Q, int dbindex);
int Find_Min(Coda *Q);
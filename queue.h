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
    int km;
    struct Nodo_C *next;
}Nodo_C;

typedef struct Coda{
    Nodo_C *tail;
    Nodo_C *head;
}Coda;


Nodo_C *CreaNodo_C(int dbindex, int km);
bool CodaVuota(Coda *Q);
void CreaCoda(Coda *Q);
void Inserisci_C(Coda *Q, int dbindex, int km);
void Eliminazione_C(Coda *Q, int dbindex, int km);
void EliminaNodo_C(Coda *Q, int dbindex);
Nodo_C *Find_Min(Coda *Q);
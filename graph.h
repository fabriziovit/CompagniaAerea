//
// Created by Fabrizio on 25/05/2020.
//
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "listAeroporti.h"

typedef struct edge {
    char citta[100];
    char codice[10];
    int km;
    int dbindex;
    int inserito;
    struct node *next;
}Edge;

typedef struct ArrayNode{
    int n;
    Edge **adj;
}Graph;

Graph *CreaGrafo(int n);
int creaNodo(Graph *P);
void Aggiungi(Graph *P, char *cittadestinazione, char *codice, int km, int insert, int dbindex, int index);
void Rimuovi (Graph *G, int indedx, char *cittadestinazione, int dbindex);
void stampaVoli(Graph *G, aeroporto L);
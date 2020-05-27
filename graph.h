//
// Created by Fabrizio on 25/05/2020.
//
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct edge {
    char citta[100];
    int km;
    struct node *next;
}Edge;

typedef struct ArrayNode{
    int n;
    Edge **adj;
}Graph;

Graph *CreaGrafo(int n);
int creaNodo(Graph *P, char *aereoporto, int km, int insert);
void Aggiungi(Graph *P, char *cittadestinazione, int km, int insert, int index);
int Rimuovi (Graph *G, char *cittapartenza, char *cittadestinazione);
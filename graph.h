//
// Created by Fabrizio on 25/05/2020.
//
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct edge {
    char aereoporto[4];
    int km;
    struct node *next;
}Edge;

typedef struct ArrayNode{
    int n;
    Edge **adj;
}Graph;

int creaNodo(Graph *P, char *aereoporto,int km, int insert);
void Aggiungi(Graph *P, char matricola[], int insert);

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
    struct edge *next;
}Edge;

typedef struct ArrayNode{
    int n;
    struct Edge **adj;
}Graph;


void creaNodo(Graph *P);
void Aggiungi(Graph *P, char cittadestinazione[], char codice[], int km, int insert, int dbindex, int index);
void Rimuovi (Graph *G, int index, char *codice);
void stampaVoli(Graph *G, aeroporto L);
int presente(Graph *G, char *codiceAeroporto, int index);
void Dijkstra(Graph *G, int src, int target, aeroporto L);
void printSP(int dist[], int n);
int getKm(Graph *G, char codiceDestinazione[10], int index);
char *TrovaMinKm(Graph *G,int index, aeroporto L);
int haTratta(Graph *G, int index);
//
// Created by Fabrizio on 25/05/2020.
//

#include "graph.h"

void creaNodo(Graph *G){
    int ind = 0;
    if (G == NULL) {
        printf("ERRORE: impossibile la lista e vuota\n");
    } else {
        /*if (G->adj == NULL){
            G->adj = (Edge **) malloc(sizeof(Edge *));
            G->adj[G->n] = NULL;
            //Aggiungi?
            ind = G->n;
            G->n++;
        }else {*/
            G->adj[G->n] = NULL;
            //Aggiungi?
            ind = G->n;
            G->n++;
        }
    }
    //return ind;


void Aggiungi(Graph *G, char cittadestinazione[], char codice[], int km, int insert,int dbindex, int index){
    Edge *new, *e;
    new = (Edge*)malloc(sizeof(Edge));
    if (new == NULL)
        printf("ERRORE: impossibile allocare memoria\n");
    else {
        strncpy(new->citta, cittadestinazione, 100);
        new->km = km;
        new->inserito = insert;
        new->dbindex = dbindex;
        strncpy(new->codice, codice,10);
        if (G->adj[index] == NULL) {
            G->adj[index] = new;
        } else {
            e = G->adj[index];
            while (e->next != NULL)
                e = e->next;
            e->next = new;
        }
        printf("Creazione tratta effettuata correttamente\n");
    }
}


void Rimuovi(Graph *G, int index, char *codice) {
    Edge *prev;
    Edge *e;
    //Ricerca nel db di cittapartenza e cerca nella lista di adiacenza adj[index] e scorrere la lista per
    e = G->adj[index];
    if (strcmp(e->codice, codice) == 0)
        G->adj[index] = e->next;
    else {
        prev = e;
        while (strcmp(prev->next->codice, codice) != 0)
            prev = prev->next;
        e = prev->next;
        prev->next = e->next;
    }
    free(e);
}

void stampaVoli(Graph *G, aeroporto L){
    int i;
    Edge *e;
    char codice[10];
    if (G != NULL) {
        for (i=0; i<G->n; i++) {
            strncpy(codice, trovaCodice(L, i+1), 10);
            e = G->adj[i];
            while (e!=NULL) {
                printf("Volo da %s -> a %s\n", codice, e->codice);
                printf("OKEYz\n");
                e = e->next;
                printf("OKEKDSA\n");
            }
            //printf("\n");
        }
    }
}
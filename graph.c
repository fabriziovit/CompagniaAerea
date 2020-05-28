//
// Created by Fabrizio on 25/05/2020.
//

#include "graph.h"

Graph *CreaGrafo(int n){
    Graph *G; int i;
    G = (Graph *)malloc(sizeof(Graph));
    if (G==NULL) printf("ERRORE: impossibile allocare memoria per il grafo\n");
    else {
        G->adj = (Edge **)malloc(n*sizeof(Edge *));
        if ((G->adj==NULL) && (n>0)) {
            printf("ERRORE: impossibile allocare memoria per la lista del grafo\n");
            free(G);
            G=NULL;}
        else {
            G->n = n;
            for (i=0; i<n; i++)
                G->adj[i]=NULL;}}
    return(G);
}

int creaNodo(Graph *G){
    int ind = 0;
    if (G == NULL) {
        printf("ERRORE: impossibile la lista e vuota\n");
    } else {
        if (G->adj == NULL){
            G->adj = (Edge **) malloc(sizeof(Edge *));
            G->adj[G->n] = NULL;
            //Aggiungi?
            ind = G->n;
            G->n++;
        }else {
            G->adj[G->n] = NULL;
            //Aggiungi?
            ind = G->n;
            G->n++;
        }
    }
    return ind;
}

void Aggiungi(Graph *G, char *cittadestinazione, int km, int insert,int dbindex, int index){
    Edge *new, *e;
    new = (Edge*)malloc(sizeof(Edge));
    if (new == NULL)
        printf("ERRORE: impossibile allocare memoria\n");
    else {
        //query per trovare l'indice dell'aeroporto
        strcpy(new->citta, cittadestinazione);
        new->km = km;
        new->inserito = insert;
        new->dbindex = dbindex;
        new->next=NULL;
        if (G->adj[index] == NULL) {
            G->adj[index] = new;
        } else {
            e = G->adj[index];
            while (e->next != NULL)
                e = e->next;
            e->next = new;
        }
    }
}

void Rimuovi(Graph *G, int index, char *cittadestinazione, int dbindex) {
    Edge *prev;
    Edge *e;
    //Ricerca nel db di cittapartenza e cerca nella lista di adiacenza adj[index] e scorrere la lista per
    e = G->adj[index];
    if (strcmp(e->citta, cittadestinazione) == 0 && dbindex == e->dbindex)
        G->adj[index] = e->next;
    else {
        prev = e;
        while (prev != NULL) {
            if(strcmp(prev->citta, cittadestinazione) && prev->dbindex == dbindex)
                break;
            prev = prev->next;
        }
        e = prev->next;
        prev->next = e->next;
    }
    free(e);
}
//
// Created by Fabrizio on 25/05/2020.
//

#include "graph.h"
#include "queue.h"

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
        new->next = NULL;
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
                e = e->next;
            }
            printf("\n");
        }
    }
}

//CONTROLLARE PER L'ULTIMA FASE
void Dijkstra(Graph *G, int src){
    int N = G->n;
    int dist[N];
    struct MinHeap *minHeap = createMinHeap(N);

    for(int n=0; n<N; ++n){
        dist[n] = INT_MAX;
        minHeap->array[n] = newMinHeapNode(n, dist[n]);
        minHeap->pos[n] = n;
    }

    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    minHeap->size = N;

    while(!isEmpty(minHeap)){
        struct MinHeapNode *minHeapNode = extractMin(minHeap);
        int u = minHeapNode->n;

        Edge *tmp = G->adj[u];
        while(tmp != NULL){
            int v = tmp->dbindex;
            if(IsInMinHeap(minHeap, v) && dist[u] != INT_MAX && tmp->km + dist[u] < dist[v]) {
                dist[v] = dist[u] + tmp->km;
                decreaseKey(minHeap, v, dist[v]);
            }
            tmp = tmp->next;
        }
    }
    printArr(dist, N);
}
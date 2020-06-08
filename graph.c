//
// Created by Fabrizio on 25/05/2020.
//

#include "graph.h"
#include "queue.h"

void creaNodo(Graph *G){
    if (G == NULL) {
        printf("ERRORE: impossibile la lista e vuota\n");
    } else {
            G->adj[G->n] = NULL;
            G->n++;
        }
    }


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
            G->adj[index] = (struct Edge *) new;
        } else {
            e = (Edge *) G->adj[index];
            while (e->next != NULL)
                e = e->next;
            e->next = new;
        }
    }
}


void Rimuovi(Graph *G, int index, char *codice) {
    Edge *prev;
    Edge *e;
    //Ricerca nel db di cittapartenza e cerca nella lista di adiacenza adj[index] e scorrere la lista per
    e = (Edge *) G->adj[index];
    if (strcmp(e->codice, codice) == 0)
        G->adj[index] = (struct Edge *) e->next;
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
            e = (Edge *) G->adj[i];
            while (e!=NULL) {
                printf("Volo da %s -> a %s\n", codice, e->codice);
                e = e->next;
            }
            printf("\n");
        }
    }
}

int presente(Graph *G, char *codiceAeroporto, int index){
    Edge *e;
    e = (Edge *) G->adj[index];
    while(e != NULL){
        if(strcmp(e->codice, codiceAeroporto) == 0)
            return 1;
        e = e->next;
    }
    return 0;
}

int getKm(Graph *G, char codiceDestinazione[10], int index){
    Edge *e;
    e = (Edge *) G->adj[index];
    while(e != NULL){
        if(strcmp(e->codice, codiceDestinazione) == 0)
            return e->km;
        e = e->next;
    }
    return -1;
}

char *TrovaMinKm(Graph *G,int index, aeroporto L){
    Edge *e;
    int min;
    int indicemin;
    e = (Edge *) G->adj[index];
    min = e->km;
    indicemin = e->dbindex;
    if(e->next!=NULL) {
        e = e->next;
        while (e != NULL) {
            if (min > e->km) {
                indicemin = e->dbindex;
                min = e->km;
                e = e->next;
            } else
                e = e->next;
        }
    }
    return trovaCodice(L, indicemin);
}

int haTratta(Graph *G, int index){
    Edge *e;
    e = (Edge *) G->adj[index];
    if(e->next)
        return 1;
    else
        return 0;
}

//FUNZIONE PER STAMPARE GLI SHORTEST PATH ALLA FINE
/*void printArr(int dist[], int n){
    for(int i=0;i<n,i++)
        printf("%d \t\t %d\n", i, dist[i]);
}*/

/*
void Dijkstra(Graph *G, int src) {
    int n = G->n;
    int dist[n];
    int prev[n];
    int vert;
    Nodo_C *tmp;
    Coda *Q;
    CreaCoda(&Q);

    for (int u = 0; u < n; u++) {  //PER OGNI VERTICE SETTARE E INSERIRE NELLA QUEUE
        dist[u] = INT_MAX;
        prev[u] = -1;
        Inserisci_C(&Q, u,G->adj[u]->km);
    }
    dist[src] = 0;

    while (!CodaVuota(&Q)) {    //FINCHE' LA CODA NON E' VUOTA TROVARE IL MIN DELLA CODA ED ESTRARLO
        tmp = Find_Min(&Q);
        vert = tmp->dbindex;
        EliminaNodo_C(&Q, vert);

        //RELAX PER OGNI VERTICE ADIACENTE AL MIN(TMP)
        for (tmp = G->adj[vert].begin(); tmp != G->adj[vert].end(); tmp = tmp->next) {
            if ((dist[vert] + tmp->km) < dist[tmp->km]) {
                dist[tmp->km] = (dist[vert] + (tmp->km));
                prev[tmp->km] = vert;
            }
        }
    }
    printArr(dist, n);
}
*/
/*struct nodoQ {
    int info;
    struct nodoQ *next;
};

int estrai_min(struct nodoQ **primo, int d[]) {
    struct nodoQ *p, *pmin, *prec=NULL, *precmin=NULL;
    int u;
    pmin = *primo;
    p = *primo;
    while (p != NULL) {
        if ((d[p->info] != 65535 && d[p->info] < d[pmin->info]) || d[pmin->info] == 65535) {
            pmin = p;
            precmin = prec;
        }
        prec = p;
        p = p->next;
    }
    u = pmin->info;

    if (precmin == NULL)
        *primo = (*primo)->next;
    else
        precmin->next = pmin->next;
    free(pmin);

    return(u);
}

void accoda(struct nodoQ **primo, int v) {
    struct nodoQ *p;
    p = malloc(sizeof(struct nodoQ));
    p->info = v;
    p->next = *primo;
    *primo = p;
    return;
}

void Dijkstra(Graph *G) {
    int d[50], pi[50];
    int n;
    int v;
    int u, s;
    aeroporto q, primo;
    Edge *e;
    n = G->n;
    for (v = 0; v < n; v++) {
        d[v] = 65535;
        pi[v] = -1;
    }

    printf("Sorgente: ");
    scanf("%d", &s);
    d[s] = 0;

    for (v = 0; v < n; v++)
        accoda(&primo, v);

    while (primo != NULL) {
        u = estrai_min(&primo, d);
        e = (Edge *) G->adj[u];
        while (e != NULL) {
            if (d[e->info] == 65535 || d[e->km] > d[u] + e->km) {
                pi[p->info] = u;
                d[p->info] = d[u] + p->w;
            }
            p = p->next;
        }
    }

    for (v = 0; v < n; v++)
        printf("d[%d] = %3d, pi[%d] = %3d\n", v, d[v], v, pi[v]);
}*/
//
// Created by Fabrizio on 25/05/2020.
//

#include "graph.h"
#define INFINITY 65000

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

int minDistance(Graph *G, int dist[], bool sptSet[]){
    // Initialize min value
    int n = G->n;
    int min = INFINITY, min_index;

    for (int v = 0; v < n; v++)
        if (sptSet[v] == false &&
            dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}


void printPercorso(aeroporto L, int parent[],int j, char *codicePartenza, int *path, int count){
    char codicePa[10];

    if (parent[j] == - 1)
        return;

    printPercorso(L, parent, parent[j], codicePartenza, path, count+1);
    strcpy(codicePa, trovaCodice(L, j+1));
    path[count] = j;
    printf("%s-", codicePa);
}

int *printSolution(aeroporto L, int dist[], int n, int parent[], int target, char *username,int *punti,int tipo, int src, double *prezzo){
    char codicePartenza[10], codiceDestinazione[10];
    double sconto;
    int *path = malloc(n*sizeof(int));
    for(int i=0;i<n;i++){
        path[i] = n+1;
    }
    printf("VOLO\t\t DISTANZA\t\tPERCORSO");
    for (int i = 1; i < n; i++){
        if(target == i) {
            *prezzo = dist[i]*2.5;
            if(tipo == 1){
                sconto = 0.1 * (*prezzo);
                *prezzo -= sconto;
            } else if(tipo == 2){
                sconto = 0.2 * (*prezzo);
                *prezzo -= sconto;
            } else if (tipo == 3){
                sconto = 0.3 * (*prezzo);
                *prezzo -= sconto;
            }else if(tipo == 4){
                sconto = 0.4 * (*prezzo);
                *prezzo -= sconto;
            } else if(tipo == 5){
                sconto = 0.5 * (*prezzo);
                *prezzo -= sconto;
            }
            strcpy(codicePartenza, trovaCodice(L, src+1));
            strcpy(codiceDestinazione, trovaCodice(L, i+1));
            printf("\n%s -> %s\t\t%d\t\t%s ",codicePartenza, codiceDestinazione, dist[i], codicePartenza);
            printPercorso(L, parent, i, codicePartenza, path, 0);
            printf("\n");
            (*punti) += dist[i]/15;
        }
    }

    return path;
}

//Funzione Dijkstra
void Dijkstra(Graph *G, int src, int target, aeroporto L, char *username, int *punti, int tipo, sqlite3 *db) {
    int n = G->n;
    int Grafo[n][n];
    int dist[n];
    bool sptset[n];
    int parent[n];
    int i, j;
    int *Path;
    Edge *tmp;
    double prezzo = 0;

    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            Grafo[i][j]=0;
        }
    }
    for(i=0;i<n;i++){
        tmp = (Edge *) G->adj[i];
        while(tmp != NULL){
            Grafo[i][tmp->dbindex-1]=tmp->km;
            tmp = tmp->next;
        }
    }
    for(i=0;i<n;i++){
        parent[src] = -1;
        dist[i] = INFINITY;
        sptset[i] = false;
    }
    dist[src] = 0;

    for(j=0; j<n-1; j++){
        int u = minDistance(G, dist, sptset);
        sptset[u] = true;
        for(int v=0;v<n;v++)
            if(!sptset[v] && Grafo[u][v] && dist[u]+ Grafo[u][v]<dist[v]){
                parent[v] = u;
                dist[v] = dist[u] + Grafo[u][v];
            }
    }
    Path = printSolution(L, dist, n, parent, target, username, punti, tipo, src, &prezzo);

    printf("\n");
    int k = n-1;
    while(Path[k] == n+1){
        k=k-1;
    }

    sqlite3_stmt *stmt;
    int rc;
    int index = 0;
    char *sql;
    int count = 0;
    sql = "INSERT INTO PRENOTATO(USERNAME, AEROPART ,AERODEST, PREZZO) VALUES(?1, ?2, ?3, ?4);";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, trovaCodice(L,src+1), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, trovaCodice(L, Path[k]+1), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4,prezzo);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if(k>=0) {
        for (i = k - 1; i >= 0; i--) {
            count++;
            sql = "INSERT INTO PRENOTATO(USERNAME, AEROPART ,AERODEST, PREZZO, SCALO) VALUES(?1, ?2, ?3, 0, TRUE);";
            sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, trovaCodice(L, Path[i+1]+1), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, trovaCodice(L, Path[i]+1), -1, SQLITE_STATIC);
            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            //Insert into PRENOTATO(username, username, prezzo, aeropart, aerodest) Values(username, 0, path[i+1], path[i])
        }
    }
    printf("Volo prenotato con successo sono presenti %d scali.\nPrezzo Totale: %.2f\n", count, prezzo);
}
//
// Created by gianl on 02/06/2020.
//

#include "queue.h"


/*Nodo_C *CreaNodo_C(int dbindex){
    Nodo_C *tmp = (Nodo_C *)malloc(sizeof(Nodo_C));

    if(!tmp)
        return NULL;
    else {
        tmp->dbindex = dbindex;
        tmp->next = NULL;
    }
    return tmp;
}

bool CodaVuota(Coda *Q){
    if(Q->tail == NULL)
        return true;
}

void CreaCoda(Coda *Q){
    if(Q){
        Q->head = NULL;
        Q->tail = NULL;
    }
}

void Inserisci_C(Coda *Q, int dbindex){
    Nodo_C *tmp;

    if(Q->tail == NULL && Q->head == NULL){
        Q->head = CreaNodo_C(dbindex);
        Q->tail = Q->head;
        return;
    }else {
        tmp = CreaNodo_C(dbindex);
        if(tmp != NULL) {
            Q->tail->next = tmp;
            Q->tail = tmp;
        }
        return;
    }
}

void EliminaNodo_C(Coda *Q, int dbindex){
    Nodo_C *tmp = Q->head;
    Nodo_C *prev;

    if(tmp != NULL && (tmp->dbindex == dbindex)){
        Q->head = tmp->next;
        free(tmp);
        return;
    }
    while(tmp != NULL && (tmp->dbindex != dbindex)){
        prev = tmp;
        tmp = tmp->next;
    }
    if(tmp == NULL) return;
    prev->next = tmp->next;
    free(tmp);
}

int Find_Min(Coda *Q){
    Nodo_C *q = Q->head;
    Nodo_C *tmp = Q->head;
    int min = tmp->dbindex;

    while(q != NULL && q->next != NULL){
        tmp = tmp->next;
        if(tmp->dbindex < q->dbindex)
            min = q->dbindex;
        else
            min = tmp->dbindex;
        q = q->next;
    }
    return min;
}
*/
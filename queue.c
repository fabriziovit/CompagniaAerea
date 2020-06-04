//
// Created by gianl on 02/06/2020.
//

#include "queue.h"


Nodo_C *CreaNodo_C(int km, int dbindex){
    Nodo_C *tmp = (Nodo_C *)malloc(sizeof(Nodo_C));

    if(!tmp)
        return NULL;
    else {
        tmp->km = km;
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

void Inserisci_C(Coda *Q, int km, int dbindex){
    Nodo_C *tmp;

    if(Q->tail == NULL && Q->head == NULL){
        Q->head = CreaNodo_C(km, dbindex);
        Q->tail = Q->head;
        return;
    }else {
        tmp = CreaNodo_C(km, dbindex);
        if(tmp != NULL) {
            Q->tail->next = tmp;
            Q->tail = tmp;
        }
        return;
    }
}

void Eliminazione_C(Coda *Q, int km, int dbindex){
    Nodo_C *tmp;

    if(Q->tail == NULL){
        printf("La coda e' vuota \n");
        return;
    }else if(Q->head == Q->tail){
        km = Q->head->km;
        dbindex = Q->head->dbindex;
        free(Q->head);
        Q->head = Q->tail = NULL;
        return;
    } else {
        tmp = Q->head;
        km = Q->head->km;
        dbindex = Q->head->dbindex;
        Q->head = Q->head->next;
        free(tmp);
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

Nodo_C  *Min(Coda *Q){
    Nodo_C *q = Q->head;
    Nodo_C *tmp = Q->head;
    int min = tmp->km;

    while(q != NULL && q->next != NULL){
        tmp = tmp->next;
        if(tmp->km < q->km)
            min = q->km;
        else
            min = tmp->km;
        q = q->next;
    }
    return min;
}

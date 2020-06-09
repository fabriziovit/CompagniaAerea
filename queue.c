//
// Created by gianl on 02/06/2020.
//

#include "queue.h"


Nodo_C *CreaNodo_C(int key){
    Nodo_C *tmp = (Nodo_C *)malloc(sizeof(Nodo_C));

    if(!tmp)
        return NULL;
    else {
        tmp->key = key;
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

void Inserisci_C(Coda *Q, int key){
    Nodo_C *tmp;

    if(Q->tail == NULL && Q->head == NULL){
        Q->head = CreaNodo_C(key);
        Q->tail = Q->head;
        return;
    }else {
        tmp = CreaNodo_C(key);
        if(tmp != NULL) {
            Q->tail->next = tmp;
            Q->tail = tmp;
        }
        return;
    }
}

void EliminaNodo_C(Coda *Q, int key){
    Nodo_C *tmp = Q->head;
    Nodo_C *prev;

    if(tmp != NULL && (tmp->key == key)){
        Q->head = tmp->next;
        free(tmp);
        return;
    }
    while(tmp != NULL && (tmp->key != key)){
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
    int min = tmp->key;

    while(q != NULL && q->next != NULL){
        tmp = tmp->next;
        if(tmp->key < q->key)
            min = q->key;
        else
            min = tmp->key;
        q = q->next;
    }
    return min;
}
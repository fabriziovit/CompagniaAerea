//
// Created by gianl on 05/06/2020.
//

/*#ifndef COMPAGNIAAEREA_LIST_H
#define COMPAGNIAAEREA_LIST_H

#endif //COMPAGNIAAEREA_LIST_H

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct Nodo_L{
    int dbindex;
    struct Nodo_L1 *next;
}Nodo_L;

typedef struct Lista{
    Nodo_L *next;
}Lista;


Nodo_L *CreaNodo_L(int dbindex){
    Nodo_L *tmp;

    tmp = (Nodo_L *)malloc(sizeof(Nodo_L));
    if(!tmp)
        return NULL;
    else{
        tmp->dbindex = dbindex;
        tmp->next = NULL;
    }
    return tmp;
}

void InserisciInTesta_L(Lista *Testa, int dbindex){
    Nodo_L *tmp = NULL;

    if(!Testa->next){
        tmp = CreaNodo_L(dbindex);
        if(tmp){
            Testa->next = tmp;
            return;
        }
    }else{
        tmp = CreaNodo_L(dbindex);

        if(tmp){
            tmp->next = Testa->next;
            Testa->next = tmp;
        }
    }
    return;
}*/
//
// Created by gianl on 09/06/2020.
//


typedef struct Nodo{
    int key;
    struct Nodo *next;
}Nodo;

typedef struct Stack{
    struct Nodo *next;
}Stack;

Nodo *CreaNodo(int key);
void Pop(Stack *Testa, int key);

Nodo *CreaNodo(int key){
    Nodo *tmp;

    tmp = (Nodo *)malloc(sizeof(Nodo));
    if(!tmp)
        return NULL;
    else{
        tmp->key = key;
        tmp->next = NULL;
    }
    return tmp;
}

void Pop(Stack *Testa, int key){
    Nodo *tmp = NULL;

    if(!Testa->next){
        tmp = CreaNodo(key);
        if(tmp){
            Testa->next = tmp;
            return;
        }
    }else{
        tmp = CreaNodo(key);
        if(tmp){
            tmp->next = Testa->next;
            Testa->next = tmp;
        }
    }
    return;
}

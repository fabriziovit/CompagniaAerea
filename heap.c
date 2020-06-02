//
// Created by gianl on 02/06/2020.
//

#include "heap.h"


struct MinHeapNode *newMinHeapNode(int n, int dist){
    struct MinHeapNode *minHeapNode = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));

    minHeapNode->n = n;
    minHeapNode->dist = dist;
    return minHeapNode;
}

struct MinHeap *createMinHeap(int cap){
    struct MinHeap *minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));

    minHeap->pos =(int*)malloc(cap * sizeof(int));
    minHeap->size = 0;
    minHeap->cap = cap;
    minHeap->array = (struct MinHeapNode**)malloc(cap * sizeof(struct MinHeapNode*));
    return minHeap;
}

void swapMinHeapNode(struct MinHeapNode **a, struct MinHeapNode **b){
    struct MinHeapNode *t = *a;
    *a = *b;
    *b = t;
}

void MinHeapify(struct MinHeap *minHeap, int idx){
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx+1;
    right = 2 * idx+2;

    if(left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;
    if(right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;
    if(smallest != idx){
        struct MinHeapNode *smallestNode = minHeap->array[smallest];
        struct MinHeapNode *idxNode = minHeap->array[idx];

        minHeap->pos[smallestNode->n] = idx;
        minHeap->pos[idxNode->n] = smallest;

        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int IsEmpty(struct MinHeap *minHeap){
    return minHeap->size == 0;
}

struct MinHeapNode *extractMin(struct MinHeap *minHeap){
    if(IsEmpty(minHeap))
        return NULL;

    struct MinHeapNode *root = minHeap->array[0];
    struct MinHeapNode *lastnode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastnode;
    minHeap->pos[root->n] = minHeap->size - 1;
    minHeap->pos[lastnode->n] = 0;
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

void drecreaseKey(struct MinHeap *minHeap, int n, int dist){
    int i = minHeap->pos[n];
    minHeap->array[i]->dist = dist;

    while(i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist){
        minHeap->pos[minHeap->array[i]->n] = (i - 1)/2;
        minHeap->pos[minHeap->array[(i - 1)/2]->n] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1)/2]);

        i = (i - 1)/2;
    }
}

bool IsInMinHeap(struct MinHeap *minHeap, int n){
    if(minHeap->pos[n] < minHeap->size)
        return true;
    return false;
}

void printArr(int dist[], int n){
    for(int i=0; i<n; ++i)
        printf("%d \t\t %d \n", i, dist[i]);
}

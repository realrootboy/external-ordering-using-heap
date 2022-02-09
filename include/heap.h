#ifndef HEAP_H
#define HEAP_H

#include "item.h"

/*
*
* REFERENCIA: Aula 15 – Filas com Prioridade
*
*/

// ESTRUTURA DE ITEM COM PESOS
typedef struct weightedItem *W_item;
// ESTRUTURA DA PQ
typedef struct priorityQueue *PQ;

// ================================================

W_item create_w_item(Item, int, int);
void destroy_w_item(W_item);

int less_w_item(W_item, W_item, int, int *);
int less_w_item_item(Item b, W_item a, int, int *);

void exch_w_item(PQ, int, int);

Item get_item_w_item(W_item);

int get_weight_w_item(W_item);

int get_device_w_item(W_item);

// ================================================

PQ PQ_init(int, int, int *); // Creates an empty priority queue.

void PQ_insert(PQ, Item, int, int); // Inserts an item in the priority queue.

// Removes and returns the largest item.
W_item PQ_delmax(PQ); // Dual op delmin also possible.

Item PQ_max(PQ); // Returns the largest item. Dual: min.

int PQ_empty(PQ); // Tests if the queue is empty.

int PQ_full(PQ);

int PQ_size(PQ); // Number of entries in the priority queue.

void PQ_finish(PQ); // Cleans up the queue.

void fix_up(PQ, int);

void fix_down(PQ, int);

void PQ_destroy(PQ);

#endif
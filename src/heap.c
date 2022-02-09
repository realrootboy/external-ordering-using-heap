#include <stdio.h>
#include <stdlib.h>

#include "../include/heap.h"

/*
* COMENTÁRIOS SOBRE A DEFINIÇÃO DAS FUNÇÕES
* PODEM SER ENCONTRADOS NO ARQUIVO DE CABEÇALHO
* heap.h
*/

struct weightedItem
{
  int weight;
  int device;
  Item item;
};

struct priorityQueue
{
  W_item *pq;
  int N;
  int size;
  int dim;
  int *indexes;
};

W_item create_w_item(Item item, int weight, int device)
{
  W_item wi = (W_item)malloc(sizeof(struct weightedItem));

  wi->item = item;
  wi->device = device;
  wi->weight = weight;

  return wi;
}

void destroy_w_item(W_item wi)
{
  if (wi != NULL)
    free(wi);
}

int less_w_item(W_item b, W_item a, int dim, int *indexes)
{
  if (a->weight != b->weight)
    return a->weight > b->weight;

  Item_ui a_c = create_item_ui(a->item, indexes, dim);
  Item_ui b_c = create_item_ui(b->item, indexes, dim);
  int cmp = compare_items_ui(a_c, b_c);

  delete_item_ui(a_c);
  delete_item_ui(b_c);
  return (cmp < 1 ? 0 : 1);
}

int less_w_item_item(Item b, W_item a, int dim, int *indexes)
{

  Item_ui a_c = create_item_ui(a->item, indexes, dim);
  Item_ui b_c = create_item_ui(b, indexes, dim);
  int cmp = compare_items_ui(a_c, b_c);

  delete_item_ui(a_c);
  delete_item_ui(b_c);
  return (cmp < 1 ? 0 : 1);
}

void exch_w_item(PQ pq, int a_key, int b_key)
{
  W_item aux = pq->pq[a_key];
  pq->pq[a_key] = pq->pq[b_key];
  pq->pq[b_key] = aux;
}

Item get_item_w_item(W_item wi)
{
  return wi->item;
}

int get_weight_w_item(W_item wi)
{
  return wi->weight;
}

int get_device_w_item(W_item wi)
{
  return wi->device;
}

// ================================================

PQ PQ_init(int size, int dim, int *indexes)
{
  PQ pq = (PQ)malloc(sizeof(struct priorityQueue));
  pq->pq = (W_item *)malloc(sizeof(W_item) * (size + 1));
  int i;
  for (i = 0; i <= size; i++)
  {
    pq->pq[i] = NULL;
  }
  pq->N = 0;
  pq->size = size;
  pq->dim = dim;
  pq->indexes = indexes;
  return pq;
}

void PQ_insert(PQ pq, Item item, int weight, int device)
{
  if (pq->N == pq->size)
    return;
  pq->N = pq->N + 1;
  pq->pq[pq->N] = create_w_item(item, weight, device);
  fix_up(pq, pq->N);
}

void fix_up(PQ pq, int k)
{
  while (k > 1 && !less_w_item(pq->pq[k / 2], pq->pq[k], pq->dim, pq->indexes))
  {
    exch_w_item(pq, k, k / 2);
    k = k / 2;
  }
}

void fix_down(PQ pq, int k)
{
  int j;
  while (2 * k <= pq->N)
  {
    j = 2 * k;
    if (j < pq->N && !less_w_item(pq->pq[j], pq->pq[j + 1], pq->dim, pq->indexes))
    {
      j++;
    }
    if (less_w_item(pq->pq[k], pq->pq[j], pq->dim, pq->indexes))
    {
      break;
    }
    exch_w_item(pq, k, j);
    k = j;
  }
}

W_item PQ_delmax(PQ pq)
{
  W_item max = pq->pq[1];

  exch_w_item(pq, 1, pq->N);
  pq->N--;
  fix_down(pq, 1);

  return max;
}

Item PQ_max(PQ pq)
{
  return pq->pq[1]->item;
}

int PQ_empty(PQ pq)
{
  return pq->N == 0;
}

int PQ_full(PQ pq)
{
  return pq->size == pq->N;
}

int PQ_size(PQ pq)
{
  return pq->N;
}

void PQ_finish(PQ pq)
{
  while (!PQ_empty(pq))
  {
    PQ_delmax(pq);
  }
}

void PQ_destroy(PQ pq)
{
  int i;

  for (i = 1; i <= pq->N; i++)
  {
    destroy_w_item(pq->pq[i]);
  }

  free(pq->pq);
  free(pq);
}

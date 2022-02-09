#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/item.h"

/*
* COMENTÁRIOS SOBRE A DEFINIÇÃO DAS FUNÇÕES
* PODEM SER ENCONTRADOS NO ARQUIVO DE CABEÇALHO
* item.h
*/

Item create_item(char *str, int m_dimension)
{
  Item i = (Item)malloc(sizeof(char *) * m_dimension);
  int idx = 0;

  const char s[2] = ",";
  char *token;

  token = strtok(str, s);
  i[idx] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
  strcpy(i[idx], token);

  token = strtok(NULL, s);
  while (token != NULL)
  {
    i[++idx] = (char *)malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(i[idx], token);
    token = strtok(NULL, s);
  }
  int len = strlen(i[idx]) - 1;

  if (i[idx][len] == '\n')
    i[idx][len] = '\0';

  return i;
}

char *at_index_item(Item i, int index)
{
  return i[index];
}

void clear_index_item(Item i, int index)
{
  i[index] = NULL;
}

void delete_item(Item i, int m_dimension)
{
  if (i == NULL)
    return;

  int j;

  for (j = 0; j < m_dimension; j++)
  {
    if (i[j] != NULL)
      free(i[j]);
  }

  free(i);
}

void print_item(Item i, int m_dimension)
{
  int j;

  for (j = 0; j < m_dimension; j++)
  {
    printf("%s ", i[j]);
  }
  printf("\n");
}

/*
* ================================================
*/

struct item_ui
{
  Item i;
  int *indexes;
  int size;
};

Item_ui create_item_ui(Item i, int *indexes, int size)
{
  Item_ui iu = (Item_ui)malloc(sizeof(struct item_ui));

  iu->i = i;
  iu->indexes = indexes;
  iu->size = size;

  return iu;
}

void set_item_item_ui(Item_ui ui, Item i)
{
  ui->i = i;
}

Item get_item_item_ui(Item_ui ui)
{
  return ui->i;
}

int *get_indexes_item_ui(Item_ui ui)
{
  return ui->indexes;
}

int get_size_item_ui(Item_ui ui)
{
  return ui->size;
}

void delete_item_ui(Item_ui ui)
{
  if (ui != NULL)
    free(ui);
}

int compare_items_ui(const void *a, const void *b)
{
  if (a == NULL)
    return -1;
  if (b == NULL)
    return 1;
  Item_ui l = ((Item_ui)a);
  Item_ui r = ((Item_ui)b);

  Item l_i = get_item_item_ui(l);
  Item r_i = get_item_item_ui(r);

  int *l_indexes = get_indexes_item_ui(l);
  int *r_indexes = get_indexes_item_ui(r);

  int size = get_size_item_ui(l);
  int cmp = 0;

  int i;

  for (i = 0; i < size; i++)
  {
    cmp = strcmp(
        at_index_item(l_i, l_indexes[i]),
        at_index_item(r_i, r_indexes[i]));
    if (cmp != 0)
      return cmp;
  }

  return cmp;
}
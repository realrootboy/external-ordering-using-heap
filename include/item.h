#ifndef ITEM_H
#define ITEM_H

// REPRESENTAÇÃO DE UMA LINHA LIDA(VETOR DE STRINGS)
typedef char **Item;

Item create_item(char *str, int m_dimension);

char *at_index_item(Item i, int index);

void clear_index_item(Item i, int index);

void delete_item(Item i, int m_dimension);

void print_item(Item i, int m_dimension);

/*
* ================================================
*/

// VETOR DE STRINGS COM INDICES(PARA COMPARAÇÃO)
typedef struct item_ui *Item_ui;

Item_ui create_item_ui(Item i, int *indexes, int size);

void set_item_item_ui(Item_ui ui, Item i);

Item get_item_item_ui(Item_ui ui);

int *get_indexes_item_ui(Item_ui ui);

int get_size_item_ui(Item_ui ui);

void delete_item_ui(Item_ui ui);

int compare_items_ui(const void *a, const void *b);

#endif
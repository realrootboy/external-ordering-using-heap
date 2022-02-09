#ifndef UTILS_H
#define UTILS_H

// Cria "n_files" arquivos e guarda em "vect3or"
void create_files(FILE **vector, int n_files);

FILE *create_file(int i);

// Fecha/libera "n_arquivos" de "vector"
void free_files(FILE **vector, int n_files);

// Conta o número de campos de uma string
// Obs:. A função não trata string vazia, retornando 1.
int count_fields(char *L1);

// Preenche o vetor de inteiros "fields" com a string "L1"
// Obs:. "," é o separador
void fill_vector(int *fields, char *L1);

void extern_sort(FILE *file_in, char *file_out, FILE **devices, int dim, int *fields, int M, int P);

void merge_files(FILE *a, FILE *b, FILE *out, int n_fields, int *fields1, int *fields2);

// Apaga todo o conteúdo de todos os P dispositivos usados para a ordenação
void erase_files_contents(FILE **devices, int P);

#endif
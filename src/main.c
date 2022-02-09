#include <stdio.h>
#include <stdlib.h>

#include "../include/utils.h"

int main(int argc, char *argv[])
{
    if (argc < 8)
        return 0;
    /*
    --- Entradas do Terminal ---
    */
    int P = atoi(argv[1]);
    int M = atoi(argv[2]);
    char *L1 = argv[3];
    char *L2 = argv[4];
    char *sfile_in1 = argv[5];
    char *sfile_in2 = argv[6];
    char *sfile_out = argv[7];
    /* -------------------------------------------
    
    Processando primeiras informações para o padrão que será utilizado
    O vetor fields armazena na primeira posição o número de campos, e nos seguintes a sequência de campos
    */
    int n_fields = count_fields(L1);
    int fields1[n_fields];
    int fields2[n_fields];

    fill_vector(fields1, L1);
    fill_vector(fields2, L2);

    FILE *file_in1 = fopen(sfile_in1, "r");
    FILE *file_in2 = fopen(sfile_in2, "r");
    char sorted_file_in1[25] = "sorted_file_in1.txt";
    char sorted_file_in2[25] = "sorted_file_in2.txt";
    FILE *file_out = fopen(sfile_out, "w");
    FILE *devices[P * 2];

    // Ordena arquivo 1 e arquivo 2
    create_files(devices, P * 2);
    extern_sort(file_in1, sorted_file_in1, devices, n_fields, fields1, M, P);
    free_files(devices, P * 2);
    // fecha os arquivos e os abre em branco para utilizá-los novamente
    create_files(devices, P * 2);
    extern_sort(file_in2, sorted_file_in2, devices, n_fields, fields2, M, P);
    /*
        Liberando arquivos e demais estruturas.
    */
    free_files(devices, P * 2);

    // Faz o merge entre o arquivo ordenado 1 e o arquivo ordenado 2
    FILE *sorted_file_1 = fopen(sorted_file_in1, "r");
    FILE *sorted_file_2 = fopen(sorted_file_in2, "r");
    merge_files(sorted_file_1, sorted_file_2, file_out, n_fields, fields1, fields2);   

    // Liberando memoria
    fclose(file_in1);
    fclose(file_in2);
    fclose(file_out);
    fclose(sorted_file_1);
    fclose(sorted_file_2);

    return 0;
}

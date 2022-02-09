#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "../include/utils.h"
#include "../include/heap.h"

/*
* COMENTÁRIOS SOBRE A DEFINIÇÃO DAS FUNÇÕES
* PODEM SER ENCONTRADOS NO ARQUIVO DE CABEÇALHO
* utils.h
*/

void extern_sort(FILE *file_in, char *file_out, FILE **devices, int dim, int *fields, int M, int P)
{
    // DIMENSÃO DO VETOR + VARIAVEL I DE CONTROLE
    unsigned int fst_dim = 0;
    unsigned int i;

    // BUFFERS E VARIAVEIS AUXILIARES DE LEITURA(GETLINE)
    char *buffer;

    size_t bufsize = 4096;
    size_t characters;
    char *buffer_item = malloc(bufsize * sizeof(char));
    memset(buffer_item, 0, bufsize * sizeof(char));
    buffer = (char *)malloc(bufsize * sizeof(char));
    if (buffer == NULL)
        return;

    // VARIAVEIS AUXILIARES QUE REPRESENTAM OS VETORES
    Item item;
    W_item last_item;
    Item item_aux;

    // LEITURA DA PRIMEIRA LINHA PARA REALIZAR A CONTAGEM
    characters = getline(&buffer, &bufsize, file_in);
    fst_dim = count_fields(buffer);
    item = create_item(buffer, fst_dim);

    // CRIAÇÃO DA PQ E ADIÇÃO DO ITEM CRIADO
    PQ pq = PQ_init(M, dim, fields);
    PQ_insert(pq, item, 0, 0);

    // VARIAVEIS PARA CONTROLE DE PESO/DEVICE
    int weight = 0;
    int principal_weight = 0;
    int current_device = 0;
    int changed = 0;

    //ENCHENDO A HEAP COM TAMANHO M
    for (;;)
    {
        if (!PQ_full(pq))
        {
            characters = getline(&buffer, &bufsize, file_in);
            if (((int)characters) == -1)
                break;

            item = create_item(buffer, fst_dim);
            PQ_insert(pq, item, weight, current_device);
        }
        else
            break;
    }

    //LENDO E DISTRIBUINDO PARA OS DEVICES
    for (;;)
    {
        last_item = PQ_delmax(pq);
        for (i = 0; i < fst_dim; i++)
        {
            weight = get_weight_w_item(last_item);
            if (weight != principal_weight)
            {
                principal_weight++;
                fprintf(devices[current_device], "|\n");
                current_device == P - 1 ? current_device = 0 : current_device++;
            }
            item_aux = get_item_w_item(last_item);
            strcat(buffer_item, item_aux[i]);
            if (i != fst_dim - 1)
                strcat(buffer_item, ",");
            // fprintf(devices[current_device], "%s,", item_aux[i]);
        }
        // fseek(devices[current_device], SEEK_CUR - 2, SEEK_CUR);
        // fprintf(devices[current_device], "\n");
        fprintf(devices[current_device], "%s\n", buffer_item);
        memset(buffer_item, 0, 4096 * sizeof(char));

        characters = getline(&buffer, &bufsize, file_in);
        if (!(((int)characters) == -1))
        {
            item = create_item(buffer, fst_dim);

            if (less_w_item_item(item, last_item, dim, fields))
            {
                weight = get_weight_w_item(last_item) + 1;
            }
            else
            {
                weight = get_weight_w_item(last_item);
            }

            PQ_insert(pq, item, weight, current_device);
        }

        delete_item(item_aux, fst_dim);
        destroy_w_item(last_item);
        if (PQ_empty(pq))
            break;
    }
    PQ_destroy(pq);

    // INTERCALAÇÃO
    pq = PQ_init(P, dim, fields);
    //variaveis de controle de intercalação
    int done = 0;
    int device_offset = 0;
    int current_device_w = P;
    current_device = 0;

    //retornando o ponteiro dos arquivos para o inicio
    for (i = 0; i < (unsigned int)2 * P; i++)
        rewind(devices[i]);

    for (; current_device < P; current_device++)
    {
        characters = getline(&buffer, &bufsize, devices[current_device]);
        if ((((int)characters) == -1) || ((buffer[0] == '|') && (strlen(buffer) == 1)))
            continue;
        item = create_item(buffer, fst_dim);
        PQ_insert(pq, item, 0, current_device);
    }
    current_device = 0;

    while (!done)
    {
        last_item = PQ_delmax(pq);
        for (i = 0; i < fst_dim; i++)
        {
            item_aux = get_item_w_item(last_item);
            strcat(buffer_item, item_aux[i]);
            if (i != fst_dim - 1)
                strcat(buffer_item, ",");
        }
        fprintf(devices[current_device_w], "%s\n", buffer_item);
        memset(buffer_item, 0, 4096 * sizeof(char));
        current_device = get_device_w_item(last_item);

        characters = getline(&buffer, &bufsize, devices[current_device]);
        if (!((((int)characters) == -1) || ((buffer[0] == '|') && (strlen(buffer) <= 2))))
        {
            item = create_item(buffer, fst_dim);
            PQ_insert(pq, item, 0, current_device);
        }
        delete_item(item_aux, fst_dim);
        destroy_w_item(last_item);

        if (PQ_empty(pq))
        {
            fprintf(devices[current_device_w], "|\n");
            current_device = device_offset;
            for (;;)
            {
                // for para iniciar a heap PQ com as primeiros elementos de cada arquivo
                for (i = 0; i < (unsigned int)P; i++)
                {
                    if (feof(devices[current_device + i]) == 0)
                    {
                        characters = getline(&buffer, &bufsize, devices[current_device + i]);
                        if (!((((int)characters) == -1) || ((buffer[0] == '|') && (strlen(buffer) <= 2))))
                        {
                            item = create_item(buffer, fst_dim);
                            PQ_insert(pq, item, 0, current_device + i);
                        }
                    }
                }
                // se após percorrer os P arquivos atuais de leitura, PQ estiver vazio, é hora de mudar para os outros 3 arquivos
                if (PQ_empty(pq))
                {
                    current_device_w -= (current_device_w % P);
                    for (i = 0; i < (unsigned int)P; i++)
                    {
                        fclose(devices[current_device + i]);
                        devices[current_device + i] = create_file(current_device + i + 1);
                        rewind(devices[current_device_w + i]);
                    }
                    current_device_w = device_offset;
                    device_offset = (device_offset + P) % (2 * P);
                    current_device = device_offset;
                    changed = 1;
                    // apagar conteúdo dos P arquivos
                }
                // caso já aconteceu a troca dos dispositivos a serem lidos e só um arquivo possui conteúdo, ele é o arquivo final
                else if (changed && PQ_size(pq) == 1)
                {
                    done = 1;
                    last_item = PQ_delmax(pq);
                    item_aux = get_item_w_item(last_item);
                    delete_item(item_aux, fst_dim);
                    destroy_w_item(last_item);
                    break;
                }
                // caso ainda haja conteúdo nos atuais arquivos de leitura
                else
                {
                    if (!changed)
                    {
                        current_device_w = ((device_offset + P) % (2 * P)) + ((current_device_w + 1) % P);
                    }
                    changed = 0;
                    break;
                }
            }
        }
    }

    int len = (int)floor(log10(current_device + 1)) + 6;
    char name[len];
    snprintf(name, sizeof(name), "%d.txt", current_device);
    rename(name, file_out);
    free(buffer);
    free(buffer_item);
    PQ_destroy(pq);
}

void merge_files(FILE *a, FILE *b, FILE *out, int n_fields, int *fields1, int *fields2)
{
    char *buffer;

    size_t bufsize = 4096;
    size_t characters;
    char *buffer_item = malloc(bufsize * sizeof(char));
    memset(buffer_item, 0, 4096 * sizeof(char));
    buffer = (char *)malloc(bufsize * sizeof(char));
    if (buffer == NULL)
        return;

    Item item_a;
    Item item_b;

    Item_ui item_ui_a;
    Item_ui item_ui_b;

    int i;

    // LEITURA DA PRIMEIRA LINHA PARA REALIZAR A CONTAGEM
    characters = getline(&buffer, &bufsize, a);
    int fst_dim = count_fields(buffer);
    item_a = create_item(buffer, fst_dim);
    item_ui_a = create_item_ui(item_a, fields1, n_fields);

    characters = getline(&buffer, &bufsize, b);
    int snd_dim = count_fields(buffer);
    item_b = create_item(buffer, snd_dim);
    item_ui_b = create_item_ui(item_b, fields2, n_fields);
    int cmp;

    for (;;)
    {
        // comparando
        cmp = compare_items_ui(item_ui_a, item_ui_b);

        // verificando igualdade
        if (cmp == 0)
        {

            for (i = 0; i < n_fields; i++)
            {
                strcat(buffer_item, item_a[fields1[i]]);
                strcat(buffer_item, ",");
                free(item_a[fields1[i]]);
                free(item_b[fields2[i]]);
                clear_index_item(item_a, fields1[i]);
                clear_index_item(item_b, fields2[i]);
            }

            for (i = 0; i < fst_dim; i++)
            {
                if (item_a[i] != NULL)
                {
                    strcat(buffer_item, item_a[i]);
                    strcat(buffer_item, ",");
                }
            }

            for (i = 0; i < snd_dim; i++)
            {
                if (item_b[i] != NULL)
                {
                    strcat(buffer_item, item_b[i]);
                    strcat(buffer_item, ",");
                }
            }

            fprintf(out, "%s", buffer_item);
            fseek(out, SEEK_CUR - 2, SEEK_CUR);
            fprintf(out, "\n");
            memset(buffer_item, 0, 4096 * sizeof(char));

            delete_item_ui(item_ui_a);
            delete_item(item_a, fst_dim);
            item_a = NULL;
            delete_item_ui(item_ui_b);
            delete_item(item_b, snd_dim);
            item_b = NULL;

            characters = getline(&buffer, &bufsize, a);
            if ((((int)characters) == -1) || ((buffer[0] == '|') && (strlen(buffer) <= 2)))
                break;
            item_a = create_item(buffer, fst_dim);
            item_ui_a = create_item_ui(item_a, fields1, n_fields);

            characters = getline(&buffer, &bufsize, b);
            if ((((int)characters) == -1) || ((buffer[0] == '|') && (strlen(buffer) <= 2)))
                break;
            item_b = create_item(buffer, snd_dim);
            item_ui_b = create_item_ui(item_b, fields2, n_fields);
        }
        else if (cmp > 0)
        {
            delete_item_ui(item_ui_b);
            delete_item(item_b, snd_dim);
            item_b = NULL;
            characters = getline(&buffer, &bufsize, b);
            if ((((int)characters) == -1) || ((buffer[0] == '|') && (strlen(buffer) <= 2)))
                break;
            item_b = create_item(buffer, snd_dim);
            item_ui_b = create_item_ui(item_b, fields2, n_fields);
            //print_item(item_b, snd_dim);
        }
        else
        {
            delete_item_ui(item_ui_a);
            delete_item(item_a, fst_dim);
            item_a = NULL;
            characters = getline(&buffer, &bufsize, a);
            if ((((int)characters) == -1) || ((buffer[0] == '|') && (strlen(buffer) <= 2)))
                break;
            item_a = create_item(buffer, fst_dim);
            item_ui_a = create_item_ui(item_a, fields1, n_fields);
            //print_item(item_a, fst_dim);
        }
    }

    if (item_a != NULL)
    {
        delete_item_ui(item_ui_a);
        delete_item(item_a, fst_dim);
    }
    if (item_b != NULL)
    {
        delete_item_ui(item_ui_b);
        delete_item(item_b, snd_dim);
    }
    free(buffer);
    free(buffer_item);
}

FILE *create_file(int i)
{
    FILE *f;
    int len = (int)floor(log10(i)) + 6;
    char name[len];
    snprintf(name, sizeof(name), "%d.txt", i - 1);
    f = fopen(name, "w+");
    return f;
}

void create_files(FILE **vector, int n_files)
{
    for (int i = 1; i <= n_files; i++)
    {
        vector[i - 1] = create_file(i);
    }
}

void free_files(FILE **vector, int n_files)
{
    for (int i = 0; i < n_files; i++)
    {
        fclose(vector[i]);
    }
}

int count_fields(char *L1)
{
    // Percorre a string da linha contando a ocorrência de vírgulas (neste caso indicam número de campos)
    int fields = 1;
    for (int i = 0; L1[i]; i++)
    {
        fields += (L1[i] == ',');
    }
    return fields;
}

void fill_vector(int *fields, char *L1)
{
    const char s[2] = ",";
    char *token;
    token = strtok(L1, s);
    int i = 0;
    while (token != NULL)
    {
        fields[i] = atoi(token);
        i++;
        token = strtok(NULL, s);
    }
}

void erase_files_contents(FILE **devices, int P)
{
    for (int i = 0; i < 2 * P; i++)
    {
        devices[i] = freopen(NULL, "w", devices[i]);
    }
}

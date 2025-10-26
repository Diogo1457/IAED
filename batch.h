/**
 * @file batch.h
 * @author Diogo Lobo (ist1109293)
 * @brief Cabeçalho para a implementação da tabela de hash de lotes de vacinas.
 * Contém definições de estruturas e declarações de funções para gerenciar a 
 * inserção, remoção e pesquisa de lotes de vacinas no sistema.
 * @date 2025-04-07
 */

#ifndef BATCH_H
#define BATCH_H

#include "date.h"

/** Tamanho máximo permitido para a tabela de hash de lotes. */
#define MAX_TABLE_SIZE 7993

/**Estrutura que contém as informações de um lote de vacina. */
typedef struct BatchInfo {
    char *batch; /** ID do lote. */
    Date date; /** Data de fabricação do lote. */
    int doses; /** Quantidade total de doses no lote. */
    int applications; /** Quantidade de doses aplicadas do lote. */
    char *vaccine_name; /** Nome da vacina associada ao lote. */
} BatchInfo;

/** Estrutura que representa um lote de vacina na tabela de hash. */
typedef struct Batches {
    char *batch_id; /** ID do lote. */
    BatchInfo* batch_info; /** Informações do lote (BatchInfo). */
    struct Batches *next; /**Ponteiro para o próximo lote na lista encadeada.*/
} Batches;

/** Estrutura que representa a tabela de hash para armazenar os lotes de 
 * vacinas. */
typedef struct BatchesHashTable {
    Batches **batches; /** Vetor de ponteiros para as listas de lotes. */
    int batch_count; /** Número de lotes armazenados na tabela. */
    int size; /** Tamanho atual da tabela de hash. */
} BatchesHashTable;

BatchesHashTable* initBatchesHashTable();

int tooManyBatchesInSystem(BatchesHashTable *batchHashTable);

int insertBatchInSystem(BatchesHashTable *hashTable, const char *batch_id, 
Date date, int doses, const char *vaccine_name);

Batches* searchBatchInSystem(BatchesHashTable *hashTable, const char *batch_id);
int validBatchNumber(BatchesHashTable *batchHashTable, const char *batch_id, 
int pt);

int listAllBatchesInSystem(BatchesHashTable *batchHashTable);

void listBatchesInSystemByGivenNames(BatchesHashTable *batchHashTable, 
    char **vaccineNames, int count, int pt);

BatchInfo* oldestExistingValidBatchByVaccineName(
BatchesHashTable* batchHashTable, char* vaccine_name, Date current_date);

void removeBatchFromSystem(BatchesHashTable *batchHashTable, 
const char *batch_id);

void destroyBatchesHashTable(BatchesHashTable *hashTable);

#endif

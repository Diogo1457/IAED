/**
 * @file batch.c
 * @author Diogo Lobo (ist1109293)
 * @brief Implementação de funções para gerenciamento de lotes 
 * de vacinas em umsistema de vacinação. Inclui operações como 
 * inserção, remoção, busca, redimensionamento da tabela hash, 
 * e listagem dos lotes.
 * @date 2025-04-07
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "batch.h"
#include "constants.h"
#include "date.h"
#include "utils.h"

/**
 * @brief Calcula o índice de hash para um identificador de lote.
 * Utiliza um algoritmo multiplicativo para calcular o valor de hash.
 * 
 * @param v O identificador do lote (batch_id).
 * @param table_size O tamanho da tabela hash.
 * 
 * @return O índice calculado para o identificador do lote na tabela hash.
 */
int hash(const char *v, int table_size) {
	int h = 0, a = 127;
	for (; *v != '\0'; v++)
		h = (a * h + *v) % table_size;
	return h;
}

/**
 * @brief Redimensiona a tabela hash de lotes para um tamanho maior.
 * Caso a carga da tabela atinja o fator máximo, a tabela é redimensionada
 * para o valor do próximo número primo, ou até o tamanho máximo permitido.
 * 
 * @param ht A tabela hash que armazena os lotes de vacina.
 * 
 * @return Retorna 1 se a tabela foi redimensionada com sucesso, ou 0
 * caso ocorra um erro de alocação de memória.
 */
int resizeBatchesHashTable(BatchesHashTable *ht) {
	unsigned int new_key;
	Batches *next;
	int new_size;
	if (ht->size == MAX_TABLE_SIZE) return 1;
	new_size = nextPrime(ht->size * 2);
	if (new_size > MAX_TABLE_SIZE) new_size = MAX_TABLE_SIZE;
	Batches **new_buckets = (Batches **)malloc(new_size*sizeof(Batches *));
	if (new_buckets == NULL) return 0;
	for (int i = 0; i < new_size; i++) new_buckets[i] = NULL;
	for (int i = 0; i < ht->size; i++) {
		Batches *current = ht->batches[i];
		while (current) {
			next = current->next;
			new_key = hash(current->batch_id, new_size);
			current->next = new_buckets[new_key];
			new_buckets[new_key] = current;
			current = next;
		}
	}
	free(ht->batches);
	ht->batches = new_buckets;
	ht->size = new_size;
	return 1;
}

/**
 * @brief Inicializa uma nova tabela hash para armazenar lotes de vacina.
 * Aloca memória para a tabela hash e seus elementos, e define o tamanho 
 * inicial da tabela e o contador de lotes.
 * 
 * @return Retorna um ponteiro para a nova tabela hash de lotes, ou NULL 
 * caso ocorra um erro de alocação de memória.
 */
BatchesHashTable* initBatchesHashTable() {
	int i;
	BatchesHashTable *batchHashTable;
	batchHashTable = (BatchesHashTable *)malloc(sizeof(BatchesHashTable));
	if (!batchHashTable) return NULL;
	batchHashTable->batches = (Batches**)malloc(INITIAL_TABLE_SIZE * 
		sizeof(Batches*));
	if (!batchHashTable->batches) {
		free(batchHashTable);
		return NULL;
	}
	for (i = 0; i < INITIAL_TABLE_SIZE; i++)
		batchHashTable->batches[i] = NULL;
	batchHashTable->batch_count = 0;
	batchHashTable->size = INITIAL_TABLE_SIZE;
	return batchHashTable;
}

/**
 * @brief Verifica se o número de lotes no sistema atingiu o limite máximo.
 * 
 * @param batchHashTable A tabela hash que armazena os lotes de vacina.
 * 
 * @return Retorna 1 se o número de lotes for maior ou igual ao limite máximo, 
 * caso contrário, retorna 0.
 */
int tooManyBatchesInSystem(BatchesHashTable *batchHashTable) {
	return batchHashTable->batch_count >= MAX_BATCHES_NUMBER;
}

/**
 * @brief Insere um novo lote de vacina no sistema.
 * 
 * @param batchHashTable A tabela hash que armazena os lotes de vacina.
 * @param batch_id O identificador do lote a ser inserido.
 * @param date A data de fabricação do lote.
 * @param doses O número de doses disponíveis no lote.
 * @param vaccine_name O nome da vacina do lote.
 * 
 * @return Retorna 1 se a inserção foi bem-sucedida, caso contrário, retorna 0.
 */
int insertBatchInSystem(BatchesHashTable *batchHashTable, const char *batch_id, 
	Date date, int doses, const char *vaccine_name) {
	Batches *new_node;
	BatchInfo *batch;
	unsigned int key;
	if ((float)batchHashTable->batch_count / batchHashTable->size 
	>= MAX_LOAD_FACTOR) {
		if (resizeBatchesHashTable(batchHashTable) == 0) return 0;
	}
	key = hash(batch_id, batchHashTable->size);
	new_node = (Batches *)malloc(sizeof(Batches));
	if (!new_node) return 0;
	batch = (BatchInfo *)malloc(sizeof(BatchInfo));
	if (!batch) {
		free(new_node);
		return 0;
	}
	batch->batch = strdup(batch_id);
	batch->date = date;
	batch->doses = doses, batch->vaccine_name = strdup(vaccine_name);
	batch->applications = 0;
	new_node->batch_id = strdup(batch_id);
	new_node->batch_info = batch;
	new_node->next = batchHashTable->batches[key];
	batchHashTable->batches[key] = new_node;
	batchHashTable->batch_count++;
	return 1;
}

/**
 * @brief Procura um lote no sistema pelo ID do lote.
 * 
 * @param batchHashTable A tabela de hash que armazena os lotes de vacinas.
 * @param batch_id O ID do lote a ser procurado.
 * 
 * @return Um ponteiro para o lote se encontrado, caso contrário NULL.
 */
Batches* searchBatchInSystem(BatchesHashTable *batchHashTable, 
	const char *batch_id) {
	unsigned int key = hash(batch_id, batchHashTable->size);
	Batches *current = batchHashTable->batches[key];
	while (current) {
		if (strcmp(current->batch_id, batch_id) == 0) {
			return current;
		}
		current = current->next;
	}
	return NULL;
}

/**
 * @brief Verifica se o número do lote é válido, ou seja, se não existe um 
 * lote com o mesmo ID no sistema.
 * 
 * @param batchHashTable A tabela de hash que armazena os lotes de vacinas.
 * @param batch_id O ID do lote a ser verificado.
 * @param pt Um valor que indica se o programa deve imprimir as mensagens de 
 * erro em português ou não.
 * 
 * @return 1 se o número do lote for válido (não duplicado), 0 se for inválido 
 * (duplicado).
 */
int validBatchNumber(BatchesHashTable *batchHashTable, const char *batch_id, 
	int pt) {
	if (searchBatchInSystem(batchHashTable, batch_id) != NULL) {
		printError(EDUPLICATEBATCHNUMBER, EDUPLICATEBATCHNUMBERPT, pt);
		return 0;
	}
	return 1;
}

/**
 * @brief Troca os valores de dois ponteiros para estruturas `BatchInfo`.
 * 
 * @param a Ponteiro para o primeiro `BatchInfo`.
 * @param b Ponteiro para o segundo `BatchInfo`.
 */
void swapBatches(BatchInfo **a, BatchInfo **b) {
	BatchInfo *temp = *a;
	*a = *b;
	*b = temp;
}

/**
 * @brief Compara dois lotes de vacina, primeiro pela data e depois pelo ID.
 * 
 * @param batch1 O primeiro `BatchInfo` a ser comparado.
 * @param batch2 O segundo `BatchInfo` a ser comparado.
 * 
 * @return Um valor negativo se `batch1` for menor que `batch2`, um valor 
 * positivo se `batch1` for maior que `batch2`, ou 0 se forem iguais.
 */
int compareBatches(BatchInfo *batch1, BatchInfo *batch2) {
	int date_cmp = compareDate1Date2(batch1->date, batch2->date);
	if (date_cmp != 0) return date_cmp;
	return strcmp(batch1->batch, batch2->batch);
}

/**
 * @brief Particiona o array de lotes para o algoritmo de ordenação rápido 
 * (quicksort).
 * 
 * @param batches O array de `BatchInfo` a ser particionado.
 * @param low O índice inicial do array a ser particionado.
 * @param high O índice final do array a ser particionado.
 * 
 * @return O índice da posição de partição no array após a reorganização.
 */
int partitionBatches(BatchInfo **batches, int low, int high) {
	BatchInfo *pivot = batches[high];
	int i = low - 1;
	for (int j = low; j < high; j++) {
		if (compareBatches(batches[j], pivot) < 0) {  
			i++;
			swapBatches(&batches[i], &batches[j]);
		}
	}
	swapBatches(&batches[i + 1], &batches[high]);
	return i + 1;
}

/**
 * @brief Realiza a ordenação rápida (quicksort) no array de lotes.
 * 
 * @param batches O array de `BatchInfo` a ser ordenado.
 * @param low O índice inicial do array a ser ordenado.
 * @param high O índice final do array a ser ordenado.
 */
void quicksortBatches(BatchInfo **batches, int low, int high) {
	if (low < high) {
		int pivotIndex = partitionBatches(batches, low, high);
		quicksortBatches(batches, low, pivotIndex - 1);
		quicksortBatches(batches, pivotIndex + 1, high);
	}
}

/**
 * @brief Imprime as informações de um lote.
 * 
 * @param batch_info A estrutura `BatchInfo` contendo as informações do lote a 
 * ser impresso.
 */
void printBatch(BatchInfo* batch_info) {
	int doses_available;
	doses_available = batch_info->doses - batch_info->applications;
	if (doses_available < 0) doses_available = 0;
	printf("%s %s %02d-%02d-%04d %d %d\n", 
		batch_info->vaccine_name, batch_info->batch,
		batch_info->date->day, batch_info->date->month, 
		batch_info->date->year, doses_available, 
		batch_info->applications);
}

/**
 * @brief Imprime as informações de todos os lotes fornecidos na lista.
 * 
 * @param batches A lista de ponteiros para os lotes a serem impressos.
 * @param batches_count O número total de lotes na lista.
 */
void printBatches(BatchInfo** batches, int batches_count) {
	for (int i = 0; i < batches_count; i++) {
		printBatch(batches[i]);
	}
	free(batches);
}

/**
 * @brief Lista todos os lotes presentes no sistema, ordenados por data e ID.
 * 
 * @param batchHashTable A tabela de hash que armazena os lotes de vacinas.
 * 
 * @return 1 se os lotes foram listados com sucesso, 0 em caso de erro.
 */
int listAllBatchesInSystem(BatchesHashTable *batchHashTable) {
	BatchInfo** batches_info, *batchInfo;
	Batches* current;
	int batches_count, i;
	batches_info = (BatchInfo**)malloc(sizeof(BatchInfo*)*
	batchHashTable->batch_count);
	if (batches_info == NULL) return 0;
	batches_count = 0;
	for (i = 0; i < batchHashTable->size; i++) {
		current = batchHashTable->batches[i];
		while (current) {
			batchInfo = current->batch_info;
			if (batchInfo) batches_info[batches_count++] = 
			current->batch_info;
			current = current->next;
		}
	}
	quicksortBatches(batches_info, 0, batches_count - 1);
	printBatches(batches_info, batches_count);
	return 1;
}

/**
 * @brief Lista os lotes presentes no sistema para os nomes de vacinas 
 * fornecidos.
 * 
 * @param batchHashTable A tabela de hash que armazena os lotes de vacinas.
 * @param vaccineNames Lista dos nomes das vacinas a serem procuradas.
 * @param count Número total de vacinas na lista.
 * @param pt Um valor que indica se o programa deve imprimir as mensagens 
 * de erro em português ou não.
 */
void listBatchesInSystemByGivenNames(BatchesHashTable *batchHashTable, 
	char **vaccineNames, int count, int pt) {
	Batches *current;
	int i, j, found;
	for (i = 1; i < count; i++) {
		found = 0;
		for (j = 0; j < batchHashTable->size; j++) {
			current = batchHashTable->batches[j];
			while (current) {
				if (strcmp(current->batch_info->vaccine_name, 
					vaccineNames[i]) == 0) {
					printBatch(current->batch_info);
					found = 1;
					break;
				}
				current = current->next;
			}
			if (found) break;
		}
		if (!found) {
			printErrorFormated(ENOSUCHVACCINE, ENOSUCHVACCINEPT, 
				pt, vaccineNames[i]);
		}
	}
}

/**
 * @brief Retorna o lote válido mais antigo de uma vacina específica que 
 * ainda tenha doses disponíveis e não tenha expirado.
 * 
 * @param batchHashTable A tabela de hash que armazena os lotes de vacinas.
 * @param vaccine_name O nome da vacina para a qual o lote será procurado.
 * @param current_date A data atual utilizada para verificar a validade dos 
 * lotes.
 * 
 * @return O lote mais antigo válido para a vacina fornecida, ou NULL se não 
 * encontrar nenhum lote válido.
 */
BatchInfo* oldestExistingValidBatchByVaccineName(
BatchesHashTable* batchHashTable, char* vaccine_name, Date current_date) {
	Batches *current;
	BatchInfo *oldest_batch = NULL, *batch_info;
	int i;
	for (i = 0; i < batchHashTable->size; i++) {
		current = batchHashTable->batches[i];
		while (current) {
			batch_info = current->batch_info;
			if (strcmp(batch_info->vaccine_name, vaccine_name) 
			== 0 && (batch_info->doses - batch_info->applications) 
			> 0 && 
			!expiredVaccineDate(current_date, batch_info->date)) {
				if (oldest_batch == NULL || 
					compareDate1Date2(batch_info->date, 
					oldest_batch->date) < 0) {
					oldest_batch = batch_info;
				}
			}
			current = current->next;
		}
	}
	return oldest_batch;
}

/**
 * @brief Libera a memória alocada para as informações de um lote de vacina.
 * 
 * @param batch_info As informações do lote a serem liberadas.
 */
void freeBatchInfo(BatchInfo* batch_info) {
	free(batch_info->batch);
	free(batch_info->vaccine_name);
	free(batch_info->date);
	free(batch_info);
}

/**
 * @brief Remove um lote do sistema baseado no seu ID.
 * 
 * @param batchHashTable A tabela de hash que armazena os lotes de vacinas.
 * @param batch_id O ID do lote a ser removido.
 */
void removeBatchFromSystem(BatchesHashTable *batchHashTable, 
	const char *batch_id) {
	unsigned int key;
	Batches *current, *previous;
	key = hash(batch_id, batchHashTable->size);
	current = batchHashTable->batches[key];
	previous = NULL;
	while (current != NULL) {
		if (strcmp(current->batch_id, batch_id) == 0) {
			if (previous == NULL) 
				batchHashTable->batches[key] = current->next;
			else previous->next = current->next;
			freeBatchInfo(current->batch_info);
			free(current->batch_id);
			free(current);
			batchHashTable->batch_count--;
			break;
		}
		previous = current;
		current = current->next;
	}
}

/**
 * @brief Destrói a tabela de hash de lotes, liberando toda a memória alocada.
 * 
 * @param batchHashTable A tabela de hash que armazena os lotes de vacinas.
 */
void destroyBatchesHashTable(BatchesHashTable *batchHashTable) {
	int i;
	Batches *current, *temp;
	if (batchHashTable == NULL) return;
	for (i = 0; i < batchHashTable->size; i++) {
		current = batchHashTable->batches[i];
		while (current) {
			temp = current;
			current = current->next;
			freeBatchInfo(temp->batch_info);
			free(temp->batch_id);
			free(temp);
		}
	}
	free(batchHashTable->batches);
	free(batchHashTable);
}

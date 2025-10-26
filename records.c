/**
 * @file records.c
 * @author Diogo Lobo (ist1109293)
 * @brief Implementação das funções para manipulação dos registros de vacinação,
 * incluindo inserção, exclusão, ordenação e consulta de registros de usuários.
 * @date 2025-04-07
 */

#include <stdlib.h>
#include <stdio.h>
#include "records.h"
#include "constants.h"
#include "utils.h"
#include "date.h"

/**
 * @brief Função hash para mapear o nome do usuário para um índice na tabela 
 * de hash.
 * 
 * @param v Nome do usuário.
 * @param table_size Tamanho da tabela de hash.
 * 
 * @return Índice gerado pela função hash.
 */
int hash_user(const char *v, int table_size) {
	int h = 0, a = 127;
	for (; *v != '\0'; v++)
		h = (a * h + *v) % table_size;
	return h;
}

/**
 * @brief Cria um registro de vacinação.
 * 
 * @param user_name Nome do usuário.
 * @param vaccine_name Nome da vacina.
 * @param batch_id Identificação do lote.
 * @param vaccination_date Data da vacinação.
 * @param id Identificador do registro.
 * 
 * @return Ponteiro para o novo registro de vacinação.
 */
VaccinationRecord* createVaccinationRecord(const char *user_name, 
	const char *vaccine_name, const char* batch_id, 
	Date vaccination_date, int id) {
	VaccinationRecord *record;
	record = (VaccinationRecord*)malloc(sizeof(VaccinationRecord));
	if (!record) return NULL;
	record->user_name = strdup(user_name);
	record->vaccine_name = strdup(vaccine_name);
	record->batch_id = strdup(batch_id);
	record->vaccination_date = vaccination_date;
	record->record_id = id;
	return record;
}

/**
 * @brief Cria um usuário de registros de vacinação.
 * 
 * @param user_name Nome do usuário.
 * 
 * @return Ponteiro para o novo usuário.
 */
VaccinationRecordsUser* createVaccinationRecordsUser(const char *user_name) {
	VaccinationRecordsUser *user = (VaccinationRecordsUser*)malloc(
		sizeof(VaccinationRecordsUser));
	if (!user) return NULL;
	user->user = strdup(user_name);
	user->records = NULL;
	user->record_count = 0;
	user->next = NULL;
	return user;
}

/**
 * @brief Inicializa a tabela de hash para os registros de vacinação.
 * 
 * @return Ponteiro para a tabela de hash inicializada.
 */
VaccinationRecordsHashtable* initVaccinationRecordsHashtable() {
	VaccinationRecordsHashtable *ht;
	int i;
	ht = (VaccinationRecordsHashtable*)malloc(
		sizeof(VaccinationRecordsHashtable));
	if (ht == NULL) return NULL;
	ht->vaccination_records = (VaccinationRecordsUser**)malloc(
		INITIAL_TABLE_SIZE * sizeof(VaccinationRecordsUser*));
	if (ht->vaccination_records == NULL) {
		free(ht);
		return NULL;
	}
	for (i = 0; i < INITIAL_TABLE_SIZE; i++)
		ht->vaccination_records[i] = NULL;
	ht->users_count = 0;
	ht->size = INITIAL_TABLE_SIZE;
	ht->all_records_count = 0;
	return ht;
}

/**
 * @brief Redimensiona a tabela de hash para um novo tamanho.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 * @param new_size Novo tamanho da tabela.
 * 
 * @return 1 se o redimensionamento foi bem-sucedido, 0 caso contrário.
 */
int resizeVaccinationRecordsHashtable(VaccinationRecordsHashtable *ht, 
	int new_size) {
	int i;
	unsigned int index;
	VaccinationRecordsUser *user, *temp;
	VaccinationRecordsUser **new_vaccination_records;
	if (new_size < INITIAL_TABLE_SIZE) return 1;
	if (new_size == ht->size) return 1;
	new_vaccination_records = (VaccinationRecordsUser**)malloc(
		new_size * sizeof(VaccinationRecordsUser*));
	if (!new_vaccination_records) return 0;
	for (i = 0; i < new_size; i++)
		new_vaccination_records[i] = NULL;
	for (i = 0; i < ht->size; i++) {
		user = ht->vaccination_records[i];
		while (user) {
			index = hash_user(user->user, new_size);
			temp = user->next;
			user->next = new_vaccination_records[index];
			new_vaccination_records[index] = user;
			user = temp;
		}
	}
	free(ht->vaccination_records);
	ht->vaccination_records = new_vaccination_records;
	ht->size = new_size;
	return 1;
}

/**
 * @brief Encontra um usuário na tabela de hash.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 * @param user_name Nome do usuário.
 * 
 * @return Ponteiro para o usuário encontrado ou NULL se não encontrado.
 */
VaccinationRecordsUser *findUser(VaccinationRecordsHashtable *ht, 
	const char *user_name) {
	unsigned int hash_index;
	hash_index = hash_user(user_name, ht->size);
	VaccinationRecordsUser *user = ht->vaccination_records[hash_index];
	while (user) {
		if (strcmp(user->user, user_name) == 0) return user;
		user = user->next;
	}
	return NULL;
}

/**
 * @brief Verifica se o usuário já existe no sistema.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 * @param user Nome do usuário.
 * 
 * @return 1 se o usuário existe, 0 caso contrário.
 */
int userExistInSystem(VaccinationRecordsHashtable *ht, char* user) {
	return findUser(ht, user) != NULL;
}

/**
 * @brief Verifica se o usuário já foi vacinado com a vacina na data informada.
 * 
 * @param user Usuário cujos registros serão verificados.
 * @param vaccine_name Nome da vacina.
 * @param date Data da vacinação.
 * 
 * @return 1 se o usuário já foi vacinado, 0 caso contrário.
 */
int isAlreadyVaccinated(VaccinationRecordsUser *user, const char *vaccine_name, 
	Date date) {
	int i;
	if (!user) return 0;
	for (i = 0; i < user->record_count; i++) {
		if (strcmp(user->records[i]->vaccine_name, vaccine_name) == 0 &&
			compareDate1Date2(user->records[i]->vaccination_date, 
				date) == 0) {
			return 1;
		}
	}
	return 0;
}

/**
 * @brief Insere um registro de vacinação para um usuário existente.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 * @param user Usuário cujos registros serão atualizados.
 * @param user_name Nome do usuário.
 * @param vaccine_name Nome da vacina.
 * @param batch_id Identificação do lote.
 * @param vaccination_date Data da vacinação.
 * 
 * @return 1 se a inserção foi bem-sucedida, 0 caso contrário.
 */
int insertIntoExistingUserRecords(VaccinationRecordsHashtable *ht,
	VaccinationRecordsUser *user, const char *user_name, 
	const char *vaccine_name, const char *batch_id, Date vaccination_date) {
	int i = 0;
	if (isAlreadyVaccinated(user, vaccine_name, vaccination_date)) return 2;
	VaccinationRecord *record = createVaccinationRecord(user_name, 
		vaccine_name, batch_id, vaccination_date, 
		ht->all_records_count);
	if (!record) return 0;
	user->records = (VaccinationRecord**)realloc(user->records, 
		sizeof(VaccinationRecord*) * (user->record_count + 1));
	if (!user->records) return 0;
	while (i < user->record_count && 
		compareDate1Date2(user->records[i]->vaccination_date, 
			vaccination_date) <= 0) {
		i++;
	}
	for (int j = user->record_count; j > i; j--)
		user->records[j] = user->records[j - 1];
	user->records[i] = record;
	user->record_count++;
	ht->all_records_count++;
	return 1;
}

/**
 * @brief Insere um novo registro de vacinação no sistema.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 * @param user_name Nome do usuário.
 * @param vaccine_name Nome da vacina.
 * @param batch_id Identificação do lote.
 * @param vaccination_date Data da vacinação.
 * 
 * @return 1 se a inserção foi bem-sucedida, 0 caso contrário.
 */
int insertVaccinationRecord(VaccinationRecordsHashtable *ht, 
	const char *user_name, const char *vaccine_name, const char* batch_id, 
	Date vaccination_date) {
	unsigned int index;
	VaccinationRecordsUser *user;
	if ((float)ht->users_count / ht->size >= MAX_LOAD_FACTOR) {
		if (resizeVaccinationRecordsHashtable(ht, 
			nextPrime(ht->size * 2)) == 0) return 0;
	}
	index = hash_user(user_name, ht->size);
	user = findUser(ht, user_name);
	if (user) { 
		return insertIntoExistingUserRecords(ht, user, user_name, 
			vaccine_name, batch_id, vaccination_date);
	}
	user = createVaccinationRecordsUser(user_name);
	if (!user) return 0;
	user->records = (VaccinationRecord**)malloc(sizeof(VaccinationRecord*));
	if (!user->records) return 0;
	user->records[0] = createVaccinationRecord(user_name, vaccine_name,
		batch_id, vaccination_date, ht->all_records_count);
	if (!user->records[0]) return 0;
	user->record_count = 1;
	user->next = ht->vaccination_records[index];
	ht->vaccination_records[index] = user;
	ht->users_count++;
	ht->all_records_count++;
	return 1;
}

/**
 * @brief Troca dois registros de vacinação.
 * 
 * @param a Ponteiro para o primeiro registro.
 * @param b Ponteiro para o segundo registro.
 */
void swap_records(VaccinationRecord **a, VaccinationRecord **b) {
	VaccinationRecord *temp = *a;
	*a = *b;
	*b = temp;
}

/**
 * @brief Compara dois registros de vacinação.
 * 
 * @param record1 Primeiro registro de vacinação.
 * @param record2 Segundo registro de vacinação.
 * 
 * @return Valor negativo se record1 for menor que record2, 
 * valor positivo se record1 for maior, 0 se forem iguais.
 */
int compare_records(VaccinationRecord* record1, VaccinationRecord* record2) {
	int date_cmp;
	date_cmp = compareDate1Date2(record1->vaccination_date, 
		record2->vaccination_date);
	if (date_cmp == 0) {
		if (record1->record_id < record2->record_id) return -1;
		if (record1->record_id > record2->record_id) return 1;
		return 0;
	}
	return date_cmp;
}

/**
 * @brief Particiona os registros de vacinação para ordenação.
 * 
 * @param records Registros de vacinação.
 * @param low Índice inferior.
 * @param high Índice superior.
 * 
 * @return Índice do pivô.
 */
int partition_records(VaccinationRecord **records, int low, int high) {
	VaccinationRecord *pivot;
	int i, j;
	pivot = records[high];
	i = low - 1;
	for (j = low; j < high; j++) {
		if (compare_records(records[j], pivot) < 0) {
			i++;
			swap_records(&records[i], &records[j]);
		}
	}
	swap_records(&records[i + 1], &records[high]);
	return i + 1;
}

/**
 * @brief Ordena os registros de vacinação usando o algoritmo QuickSort.
 * 
 * @param records Registros de vacinação.
 * @param low Índice inferior.
 * @param high Índice superior.
 */
void quicksort_records(VaccinationRecord **records, int low, int high) {
	int pivotIndex;
	if (low < high) {
		pivotIndex = partition_records(records, low, high);
		quicksort_records(records, low, pivotIndex - 1);
		quicksort_records(records, pivotIndex + 1, high);
	}
}

/**
 * @brief Imprime um registro de vacinação.
 * 
 * @param record O registro de vacinação a ser impresso.
 */
void print_record(VaccinationRecord *record) {
	printf("%s %s %02d-%02d-%04d\n", 
		record->user_name, record->batch_id, 
		record->vaccination_date->day, 
		record->vaccination_date->month, 
		record->vaccination_date->year);
}

/**
 * @brief Lista todos os registros de vacinação no sistema.
 * 
 * @param vaccinationSystem Tabela de hash com os registros de vacinação.
 * 
 * @return 1 se a operação for bem-sucedida, 0 caso contrário.
 */
int listAllRecordsInSystem(VaccinationRecordsHashtable* vaccinationSystem) {
	VaccinationRecord **all_records = NULL;
	VaccinationRecordsUser *user;
	int i, j, index, sys_records_num;
	sys_records_num = vaccinationSystem->all_records_count;
	all_records = (VaccinationRecord**)malloc(
		sizeof(VaccinationRecord*) * sys_records_num);
	if (all_records == NULL) return 0;
	index  = 0;
	for (i = 0; i < vaccinationSystem->size; i++) {
		user = vaccinationSystem->vaccination_records[i];
		while (user != NULL) {
			for (j = 0; j < user->record_count; j++) {
				all_records[index++] = user->records[j];
			}
			user = user->next;
		}
	}
	quicksort_records(all_records, 0, sys_records_num - 1);
	for (i = 0; i < sys_records_num; i++)
		print_record(all_records[i]);
		
	free(all_records);
	return 1;
}

/**
 * @brief Lista todos os registros de vacinação de um usuário no sistema.
 * 
 * @param vaccinationSystem Tabela de hash com os registros de vacinação.
 * @param name Nome do usuário.
 */
void listAllUserRecordsInSystem(VaccinationRecordsHashtable *vaccinationSystem, 
	const char *name) {
	VaccinationRecordsUser *user;
	user = findUser(vaccinationSystem, name);
	for (int i = 0; i < user->record_count; i++)
		print_record(user->records[i]);
}

/**
 * @brief Libera a memória de um registro de vacinação.
 * 
 * @param record O registro de vacinação a ser liberado.
 */
void freeVaccinationRecord(VaccinationRecord* record) {
	if (record != NULL) {
		free(record->user_name);
		free(record->vaccine_name);
		free(record->batch_id);
		free(record->vaccination_date);
		free(record);
	}
}

/**
 * @brief Exclui todos os registros de vacinação de um usuário do sistema.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 * @param user_name Nome do usuário.
 * 
 * @return O número de registros excluídos.
 */
int deleteRecordVaccinationRecordsUser(VaccinationRecordsHashtable *ht, 
	const char *user_name) {
	VaccinationRecordsUser *prev, *curr;
	int deleted = 0;
	unsigned int index = hash_user(user_name, ht->size);
	prev = NULL;
	curr = ht->vaccination_records[index];
	while (curr) {
		if (strcmp(curr->user, user_name) == 0) {
			for (int i = 0; i < curr->record_count; i++) {
				freeVaccinationRecord(curr->records[i]);
				ht->all_records_count--;
				deleted++;
			}
			if (prev) prev->next = curr->next;
			else ht->vaccination_records[index] = curr->next;
			free(curr->records);
			free(curr->user);
			free(curr);
			break;
		}
		prev = curr;
		curr = curr->next;
	}
	ht->users_count--;
	if ((float)ht->users_count / ht->size < (1-MAX_LOAD_FACTOR))
		if (resizeVaccinationRecordsHashtable(ht, 
			nextPrime(ht->size/2)) == 0) return 0;
	return deleted;
}

/**
 * @brief Exclui um registro de vacinação de um usuário com base na data.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 * @param user_name Nome do usuário.
 * @param vaccination_date Data da vacinação.
 * 
 * @return O número de registros excluídos.
 */
int deleteRecordByNameAndDate(VaccinationRecordsHashtable *ht, 
	const char *user_name, Date vaccination_date) {
	VaccinationRecordsUser *user;
	user = findUser(ht, user_name);
	int count = 0, deleted = 0;
	VaccinationRecord **new_records = (VaccinationRecord **)malloc(
		sizeof(VaccinationRecord*) * user->record_count);
	if (new_records == NULL) return -1;
	for (int i = 0; i < user->record_count; i++) {
		if (compareDate1Date2(user->records[i]->vaccination_date, 
			vaccination_date) == 0) {
			freeVaccinationRecord(user->records[i]);
			ht->all_records_count--;
			deleted++;
		} 
		else new_records[count++] = user->records[i];
	}
	free(user->records);
	user->records = new_records;
	user->record_count = count;
	if (user->record_count == 0) deleteRecordVaccinationRecordsUser(ht, 
		user_name);
	return deleted;
}

/**
 * @brief Exclui um registro de vacinação de um usuário 
 * com base na data e no lote.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 * @param user_name Nome do usuário.
 * @param vaccination_date Data da vacinação.
 * @param batch_id Identificação do lote.
 * 
 * @return O número de registros excluídos.
 */
int deleteRecordByNameDateAndBatchID(VaccinationRecordsHashtable *ht, 
	const char *user_name, Date vaccination_date, const char *batch_id) {
	VaccinationRecordsUser *user;
	int count = 0, deleted = 0;
	user = findUser(ht, user_name);
	VaccinationRecord **new_records = (VaccinationRecord **)malloc(
		sizeof(VaccinationRecord*) * user->record_count);
	if (new_records == NULL) return -1;
	for (int i = 0; i < user->record_count; i++) {
		if (compareDate1Date2(user->records[i]->vaccination_date, 
			vaccination_date) == 0 &&
			strcmp(user->records[i]->batch_id, batch_id) == 0) {
			freeVaccinationRecord(user->records[i]);
			ht->all_records_count--;
			deleted++;
		} else new_records[count++] = user->records[i];
	}
	free(user->records);
	user->records = new_records;
	user->record_count = count;
	if (user->record_count == 0) 
		deleteRecordVaccinationRecordsUser(ht, user_name);
	return deleted;
}

/**
 * @brief Libera a memória utilizada pela tabela de hash de registros de 
 * vacinação.
 * 
 * @param ht Tabela de hash de registros de vacinação.
 */
void destroyVaccinationRecordsHashtable(VaccinationRecordsHashtable *ht) {
	int i, j;
	VaccinationRecordsUser *user, *temp;
	if (ht == NULL || ht->vaccination_records == NULL) return;
	for (i = 0; i < ht->size; i++) {
		user = ht->vaccination_records[i];
		while (user) {
			for (j = 0; j < user->record_count; j++)
				freeVaccinationRecord(user->records[j]);
			free(user->records);
			free(user->user);
			temp = user;
			user = user->next;
			free(temp);
		}
	}
	free(ht->vaccination_records);
	free(ht);
}

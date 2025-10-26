/**
 * @file system.c
 * @author Diogo Lobo (ist1109293)
 * @brief Implementação de funções para inicialização e destruição do sistema 
 * de vacinação
 * @date 2025-04-07
 */
#include <stdlib.h>
#include "system.h"
#include "batch.h"
#include "records.h"
#include "constants.h"

/**
 * @brief Libera a memória alocada para o sistema de vacinação e a data
 * 
 * @param vs Sistema de vacinação a ser desalocado
 * @param date Data a ser desalocada
 */
void freeVaccionationSystemAndDate(VaccinationSystem* vs, Date date) {
	free(vs);
	free(date);
}

/**
 * @brief Inicializa o sistema de vacinação
 * 
 * Esta função aloca memória e inicializa as tabelas hash para os registros de 
 * vacinação e os lotes, além de definir a data atual do sistema para 
 * 01/01/2025.
 * 
 * @return Um ponteiro para o sistema de vacinação inicializado, 
 * ou NULL em caso de falha
 */
VaccinationSystem* initVaccinationSystem() {
	VaccinationSystem* vaccination_system = NULL;
	BatchesHashTable *batches_ht = NULL;
	VaccinationRecordsHashtable *records_ht = NULL;
	Date current_date;
	vaccination_system = (VaccinationSystem*)malloc(
		sizeof(VaccinationSystem));
	if (vaccination_system == NULL) return NULL;
	current_date = (Date) malloc(sizeof(struct Date));
	if (current_date == NULL) {
		free(vaccination_system);
		return NULL;
	}
	current_date->day = 1,current_date->month = 1,current_date->year = 2025;
	vaccination_system->current_date = current_date;
	batches_ht = initBatchesHashTable();
	if (batches_ht == NULL) {
		freeVaccionationSystemAndDate(vaccination_system, current_date);
		return NULL;
	}
	vaccination_system->batches_ht = batches_ht;
	records_ht = initVaccinationRecordsHashtable();
	if (records_ht == NULL) {
		freeVaccionationSystemAndDate(vaccination_system, current_date);
		destroyBatchesHashTable(batches_ht);
		return NULL;
	}
	vaccination_system->records_ht = records_ht;
	return vaccination_system;
}

/**
 * @brief Destrói o sistema de vacinação
 * 
 * Libera a memória alocada para o sistema de vacinação, destruindo as tabelas 
 * hash associadas e liberando a memória da data atual.
 * 
 * @param vaccinationSystem Sistema de vacinação a ser destruído
 */
void destroyVaccinationSystem(VaccinationSystem* vaccinationSystem) {
	if (vaccinationSystem == NULL) return;
	destroyBatchesHashTable(vaccinationSystem->batches_ht);
	destroyVaccinationRecordsHashtable(vaccinationSystem->records_ht);
	free(vaccinationSystem->current_date);
	free(vaccinationSystem);
}

/**
 * @file system.h
 * @author Diogo Lobo (ist1109293)
 * @brief Definições e declarações de funções para o sistema 
 * de vacinação
 * @date 2025-04-07
 * 
 */
#ifndef SYSTEM_H
#define SYSTEM_H

#include "batch.h"
#include "records.h"

/**
 * @brief Estrutura que representa o sistema de vacinação
 * 
 * Contém as tabelas hash para os lotes de vacinas, os registros de vacinação 
 * e a data atual do sistema.
 */
typedef struct VaccinationSystem {
    BatchesHashTable* batches_ht; /** Tabela hash para os lotes de vacina */
    VaccinationRecordsHashtable* records_ht; /** Tabela hash para os 
    registros de vacinação */
    Date current_date; /** Data atual do sistema de vacinação */
} VaccinationSystem;

VaccinationSystem* initVaccinationSystem();
void destroyVaccinationSystem(VaccinationSystem* vaccinationSystem);

#endif

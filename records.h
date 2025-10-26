/**
 * @file records.h
 * @author Diogo Lobo (ist1109293)
 * @brief Arquivo de cabeçalho para gerenciar registros de vacinação em um 
 * sistema usando uma tabela hash
 * @date 2025-04-07 
 */

#ifndef RECORDS_H
#define RECORDS_H

#include "string.h"
#include "date.h"

/**
 * Estrutura que representa um registro de vacinação de um usuário
 */
typedef struct VaccinationRecord {
    int record_id; /** ID único do registro de vacinação */
    char *user_name; /** Nome do usuário que recebeu a vacina */
    char *vaccine_name; /** Nome da vacina administrada */
    char *batch_id; /** Identificador do lote da vacina */
    Date vaccination_date; /** Data da vacinação */
} VaccinationRecord;

/**
 * Estrutura que representa um conjunto de registros de vacinação de um usuário
 */
typedef struct VaccinationRecords {
    char *user_name; /** Nome do usuário */
    VaccinationRecord* record; /** Registro de vacinação associado ao usuário */
    struct VaccinationRecords *next;  /** Ponteiro para o próximo registro de 
    vacinação */
} VaccinationRecords;

/**
 * Estrutura que representa todos os registros de vacinação de um único usuário
 */
typedef struct VaccinationRecordsUser {
    char *user; /** Nome do usuário */
    VaccinationRecord **records; /** Lista de registros de 
    vacinação do usuário */
    int record_count; /** Número de registros de vacinação do usuário */
    struct VaccinationRecordsUser *next; /** Ponteiro para o próximo usuário */
} VaccinationRecordsUser;

/**
 * Estrutura que representa uma tabela hash para armazenar registros de 
 * vacinação
 */
typedef struct VaccinationRecordsHashtable {
    VaccinationRecordsUser **vaccination_records; /** Tabela hash para armazenar
    os registros de usuários */
    int users_count; /** Número de usuários cadastrados no sistema */
    int all_records_count; /** Número total de registros de vacinação */
    int size; /** Tamanho da tabela hash */
} VaccinationRecordsHashtable;

VaccinationRecordsHashtable* initVaccinationRecordsHashtable();

int insertVaccinationRecord(VaccinationRecordsHashtable *ht, 
const char *user_name, const char *vaccine_name, const char* batch_id, 
Date vaccination_date);

int userExistInSystem(VaccinationRecordsHashtable *ht, char* user);

int listAllRecordsInSystem(VaccinationRecordsHashtable* vaccinationSystem);

void listAllUserRecordsInSystem(VaccinationRecordsHashtable *vaccinationSystem, 
const char *name);

int deleteRecordVaccinationRecordsUser(VaccinationRecordsHashtable *ht, 
const char *user_name);

int deleteRecordByNameAndDate(VaccinationRecordsHashtable *ht, 
const char *user_name, Date vaccination_date);

int deleteRecordByNameDateAndBatchID(VaccinationRecordsHashtable *ht, 
const char *user_name, Date vaccination_date, const char *batch_id);

void destroyVaccinationRecordsHashtable(VaccinationRecordsHashtable *ht);

#endif

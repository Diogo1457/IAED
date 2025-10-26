/**
 * @file utils.h
 * @author Diogo Lobo (ist1109293)
 * @brief Arquivo de cabeçalho para funções utilitárias usadas no sistema de 
 * vacinação.
 * @date 2025-04-07
 */

#ifndef UTILS_H
#define UTILS_H

void printError(const char* error, const char* error_pt, int pt);
void printErrorFormated(const char* error,  
const char* error_pt, int pt, char* info);
int validBatch(char* batch, int num_args, int pt);
int validName(char* name, int num_args, int pt);
int validDosesNumber(int doses_number, int pt);
int nextPrime(int num);
int countArguments(const char *input);

#endif

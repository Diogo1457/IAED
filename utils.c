/**
 * @file utils.c
 * @author Diogo Lobo (ist1109293)
 * @brief Implementação de funções auxiliares para o 
 * sistema de vacinação
 * @date 2025-04-07
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "constants.h"

/**
 * @brief Imprime uma mensagem de erro
 * 
 * Exibe uma mensagem de erro dependendo do idioma (pt ou não).
 * 
 * @param error Mensagem de erro em inglês
 * @param error_pt Mensagem de erro em português
 * @param pt Indica se o idioma é português (1) ou inglês (0)
 */
void printError(const char* error, const char* error_pt, int pt) {
	!pt ? puts(error) : puts(error_pt);
}

/**
 * @brief Imprime uma mensagem de erro formatada
 * 
 * Exibe uma mensagem de erro formatada com informações adicionais.
 * 
 * @param error Mensagem de erro em inglês
 * @param error_pt Mensagem de erro em português
 * @param pt Indica se o idioma é português (1) ou inglês (0)
 * @param info Informação adicional para exibir na mensagem de erro
 */
void printErrorFormated(const char* error, 
	const char* error_pt, int pt, char* info) {
	printf("%s: %s\n", info, !pt ? error : error_pt);
}

/**
 * @brief Valida se o lote informado é válido
 * 
 * Verifica se o nome do lote tem o formato correto e está dentro do comprimento
 * esperado.
 * 
 * @param batch Nome do lote a ser validado
 * @param num_args Número de argumentos fornecidos
 * @param pt Indica se o idioma é português (1) ou inglês (0)
 * 
 * @return 1 se o lote for válido, 0 caso contrário
 */
int validBatch(char* batch, int num_args, int pt) {
	int i, length;
	if (num_args != 6) {
		printError(EINVALIDBATCH, EINVALIDBATCHPT, pt);
		return 0;
	}
	length = strlen(batch);
	if (length == 0 || length > MAX_BATCH_NAME_SIZE) {
		printError(EINVALIDBATCH, EINVALIDBATCHPT, pt);
		return 0;
	}
	for (i = 0; batch[i] != '\0'; i++) {
		if (!((batch[i] >= '0' && batch[i] <= '9') || 
			(batch[i] >= 'A' && batch[i] <= 'F'))) {
			printError(EINVALIDBATCH, EINVALIDBATCHPT, pt);
			return 0;
		}
	}
	return 1;
}

/**
 * @brief Valida o nome da vacina informado
 * 
 * Verifica se o nome da vacina tem o formato correto, 
 * sem espaços ou caracteres inválidos.
 * 
 * @param name Nome da vacina a ser validado
 * @param num_args Número de argumentos fornecidos
 * @param pt Indica se o idioma é português (1) ou inglês (0)
 * 
 * @return 1 se o nome for válido, 0 caso contrário
 */
int validName(char* name, int num_args, int pt) {
	int i, length, slash_found = 0;
	if (num_args != 6) {
		printError(EINVALIDNAME, EINVALIDNAMEPT, pt);
		return 0;
	}
	length = strlen(name);
	if (length == 0 || length > MAX_VACCINE_NAME_SIZE) {
		printError(EINVALIDNAME, EINVALIDNAMEPT, pt);
		return 0;
	}
	for (i = 0; name[i] != '\0'; i++) {
		if (isspace(name[i])) {
			printError(EINVALIDNAME, EINVALIDNAMEPT, pt);
			return 0;
		}
		if (slash_found) {
			if (name[i] == 'n' || name[i] == 't') {
				printError(EINVALIDNAME, EINVALIDNAMEPT, pt);
				return 0;
			}
			slash_found = 0;
		}
		if (name[i] == '\\') slash_found = 1;
	}
	return 1;
}

/**
 * @brief Valida o número de doses informado
 * 
 * Verifica se o número de doses é válido (não negativo).
 * 
 * @param doses_number Número de doses a ser validado
 * @param pt Indica se o idioma é português (1) ou inglês (0)
 * 
 * @return 1 se o número de doses for válido, 0 caso contrário
 */
int validDosesNumber(int doses_number, int pt) {
	if (doses_number < 0) {
		printError(EINVALIDQUANTITY, EINVALIDQUANTITYPT, pt);
		return 0;
	}
	return 1;
}

/**
 * @brief Verifica se um número é primo
 * 
 * @param num Número a ser verificado
 * 
 * @return 1 se o número for primo, 0 caso contrário
 */
int isPrime(int num) {
	if (num <= 1) return 0;
	for (int i = 2; i * i <= num; i++)
		if (num % i == 0) return 0;
	return 1;
}

/**
 * @brief Encontra o próximo número primo
 * 
 * Retorna o próximo número primo após o número fornecido. Essa função é 
 * utilizada para calcular o próximo tamanho primo necessário ao redimensionar 
 * a tabela hash.
 * 
 * @param num Primo anterior
 * 
 * @return O próximo número primo
 */
int nextPrime(int num) {
	while (!isPrime(num)) num++;
	return num;
}

/**
 * @brief Conta o número de argumentos em uma string
 * 
 * Conta o número de palavras (argumentos) em uma string, 
 * levando em consideração as aspas.
 * 
 * @param input String contendo os argumentos
 * 
 * @return O número de argumentos na string
 */
int countArguments(const char *input) {
	int i, inside_quotes = 0, arg_count = 0, in_word = 0;
	for (i = 0; input[i] != '\0'; i++) {
		if (input[i] == '"') {
			inside_quotes = !inside_quotes;
			if (!in_word) {
				arg_count++;
				in_word = 1;
			}
		} else if (isspace(input[i]) && !inside_quotes)
			in_word = 0;
		else {
			if (!in_word) {
				arg_count++;
				in_word = 1;
			}
		}
	}
	return arg_count - 1;
}

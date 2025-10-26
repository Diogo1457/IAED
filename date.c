/**
 * @file date.c
 * @author Diogo Lobo (ist1109293)
 * @brief Implementa funções relacionadas a datas, como criação, 
 * cópia, comparação, verificação de ano bissexto, e validação de datas.
 * @date 2025-04-07
 */

#include <stdlib.h>
#include "date.h"
#include "constants.h"
#include "utils.h"


/**
 * @brief Cria uma nova data com os valores fornecidos para o dia, mês e ano.
 * 
 * @param day O dia da data.
 * @param month O mês da data.
 * @param year O ano da data.
 * 
 * @return Uma nova estrutura de data com os valores fornecidos, 
 * ou NULL se falhar.
 */
Date createDate(int day, int month, int year) {
	Date date;
	date = (Date)malloc(sizeof(struct Date));
	if (date != NULL) {
		date->day = day;
		date->month = month;
		date->year = year;
	}
	return date;
}

/**
 * @brief Cria uma cópia de uma data existente.
 * 
 * @param date A data a ser copiada.
 * 
 * @return Uma nova estrutura de data com os 
 * mesmos valores da data fornecida, ou NULL se falhar.
 */
Date copyDate(Date date) {
	Date new_date;
	new_date = (Date)malloc(sizeof(struct Date));
	if (new_date != NULL) {
		new_date->day = date->day;
		new_date->month = date->month;
		new_date->year = date->year;
	}
	return new_date;
}

/**
 * @brief Verifica se um ano é bissexto.
 * 
 * @param year O ano a ser verificado.
 * 
 * @return 1 se o ano for bissexto, 0 caso contrário.
 */
int isLeapYear(int year) {
	return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

/**
 * @brief Retorna o número de dias no mês especificado para um ano específico.
 * 
 * @param month O mês a ser verificado.
 * @param year O ano do qual o número de dias será verificado.
 * 
 * @return O número de dias no mês especificado.
 */
int days_of_month(int month, int year) {
	switch (month) {
		case APR:
		case JUNE:
		case SEPT:
		case NOV:
			return 30;
		case FEB:
			return isLeapYear(year) ? 29 : 28;
		case OCT:
			return (year == 1582) ? 21 : 31;
		default:
			return 31;
	}
}

/**
 * @brief Compara duas datas para determinar qual é anterior.
 * 
 * @param date1 A primeira data a ser comparada.
 * @param date2 A segunda data a ser comparada.
 * 
 * @return 1 se a data1 for posterior a date2, -1 se for anterior, 
 * 0 se forem iguais.
 */
int compareDate1Date2(Date date1, Date date2) {
	if (date1->year > date2->year) return 1;
	if (date1->year < date2->year) return -1;

	if (date1->month > date2->month) return 1;
	if (date1->month < date2->month) return -1;

	if (date1->day > date2->day) return 1;
	if (date1->day < date2->day) return -1;
	return 0;
}

/**
 * @brief Verifica se uma vacina está vencida em relação à data do sistema.
 * 
 * @param system_date A data atual do sistema.
 * @param date A data de validade da vacina.
 * 
 * @return 1 se a vacina estiver vencida, 0 caso contrário.
 */
int expiredVaccineDate(Date system_date, Date date) {
	return compareDate1Date2(system_date, date) > 0;
}

/**
 * @brief Valida se uma data fornecida é válida, verificando o ano, mês, dia 
 * e se não está expirada.
 * 
 * @param system_date A data atual do sistema.
 * @param date A data a ser validada.
 * @param pt Um valor que indica se a mensagem de erro será impressa em 
 * português ou não.
 * 
 * @return 1 se a data for válida, 0 caso contrário.
 */
int validDate(Date system_date, Date date, int pt) {
	if (date->year < 0 || 
		!(1 <= date->month && date->month <= 12) || 
		!(1 <= date->day && 
			date->day <= days_of_month(date->month, date->year)) ||
		expiredVaccineDate(system_date, date)) {
		printError(EINVALIDDATE, EINVALIDDATEPT, pt);
		return 0;
	}
	return 1;
}

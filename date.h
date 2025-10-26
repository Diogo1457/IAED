/**
 * @file date.h
 * @author Diogo Lobo (ist1109293)
 * @brief Arquivo de cabeçalho para definição da estrutura de data e funções 
 * associadas para manipulação, comparação, verificação de validade e expiração 
 * de datas.
 * @date 2025-04-07
 * 
 */
#ifndef DATE_H
#define DATE_H


/** Estrutura que representa uma data com dia, mês e ano. */
struct Date {
    int day; /** O dia da data. */
    int month; /** O mês da data. */
    int year; /** O ano da data. */
};

/** Enumeração dos meses do ano. */
enum Meses{JAN=1, FEB, MAR, APR, MAY, JUNE, JULY, AUG, SEPT, OCT, NOV, DEC};

/** Tipo Date, que é um ponteiro para a estrutura Date. */
typedef struct Date* Date;

Date copyDate(Date date);
int compareDate1Date2(Date date1, Date date2);
int expiredVaccineDate(Date system_date, Date date);
int validDate(Date system_date, Date date, int pt);

#endif

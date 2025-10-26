/**
 * @file constants.h
 * @author Diogo Lobo (ist1109293)
 * @brief Define constantes utilizadas no sistema, como tamanhos de buffer, 
 * números máximos e mensagens de erro.
 * @date 2025-04-07
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/** Tamanho do buffer para leitura de dados. */
#define BUFFER_SIZE 65535

/** Tamanho máximo permitido para o nome de uma vacina. */
#define MAX_VACCINE_NAME_SIZE 50

/** Tamanho máximo permitido para o nome de um lote. */
#define MAX_BATCH_NAME_SIZE 20

/** Número máximo de lotes que o sistema pode armazenar. */
#define MAX_BATCHES_NUMBER 1000

/** Comprimento máximo permitido para o nome de um usuário. */
#define MAX_USER_LENGTH 200

/** Fator de carga máximo da tabela de hash. */
#define MAX_LOAD_FACTOR 0.75

/** Tamanho inicial da tabela de hash. */
#define INITIAL_TABLE_SIZE 101

/** Argumento para indicar o idioma português. */
#define PT_LANG_ARGUMENT "pt"

/** Mensagem de erro para falta de memória. */
#define ENOMEMORY "No memory"
/** Mensagem de erro para falta de memória (em português). */
#define ENOMEMORYPT "sem memória"

/** Mensagem de erro para lote inválido. */
#define EINVALIDBATCH "invalid batch"
/** Mensagem de erro para lote inválido (em português). */
#define EINVALIDBATCHPT "lote inválido"

/** Mensagem de erro para nome inválido. */
#define EINVALIDNAME "invalid name"
/** Mensagem de erro para nome inválido (em português). */
#define EINVALIDNAMEPT "nome inválido"

/** Mensagem de erro para data inválida. */
#define EINVALIDDATE "invalid date"
/** Mensagem de erro para data inválida (em português). */
#define EINVALIDDATEPT "data inválida"

/** Mensagem de erro para quantidade inválida. */
#define EINVALIDQUANTITY "invalid quantity"
/** Mensagem de erro para quantidade inválida (em português). */
#define EINVALIDQUANTITYPT "quantidade inválida"

/** Mensagem de erro para excesso de vacinas. */
#define ETOOMANYVACCINES "too many vaccines"
/** Mensagem de erro para excesso de vacinas (em português). */
#define ETOOMANYVACCINESPT "demasiadas vacinas"

/** Mensagem de erro para número de lote duplicado. */
#define EDUPLICATEBATCHNUMBER "duplicate batch number"
/** Mensagem de erro para número de lote duplicado (em português). */
#define EDUPLICATEBATCHNUMBERPT "número de lote duplicado"

/** Mensagem de erro para vacina inexistente. */
#define ENOSUCHVACCINE "no such vaccine"
/** Mensagem de erro para vacina inexistente (em português). */
#define ENOSUCHVACCINEPT "vacina inexistente"

/** Mensagem de erro para falta de estoque. */
#define ENOSTOCK "no stock"
/** Mensagem de erro para falta de estoque (em português). */
#define ENOSTOCKPT "esgotado"

/** Mensagem de erro para já ter sido vacinado. */
#define EALREADYVACCINATED "already vaccinated"
/** Mensagem de erro para já ter sido vacinado (em português). */
#define EALREADYVACCINATEDPT "já vacinado"

/** Mensagem de erro para lote inexistente. */
#define ENOSUCHBATCH "no such batch"
/** Mensagem de erro para lote inexistente (em português). */
#define ENOSUCHBATCHPT "lote inexistente"

/** Mensagem de erro para usuário inexistente. */
#define ENOSUCHUSER "no such user"
/** Mensagem de erro para usuário inexistente (em português). */
#define ENOSUCHUSERPT "utente inexistente"

#endif

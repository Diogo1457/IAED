/* iaed25 - ist1109293 - project */
/**
 * @file project.c
 * @author Diogo Lobo (ist1109293)
 * @brief Implementa um sistema de gerenciamento de vacinação 
 * via linha de comando, que lida com o registro de lotes de vacinas, 
 * rastreamento de aplicações, registros de vacinação de usuários e 
 * progressão de datas. Suporta a listagem, adição, remoção de lotes e 
 * registros, e gerencia validações de memória e entrada
 * @date 2025-04-07
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "constants.h"
#include "utils.h"
#include "date.h"
#include "batch.h"
#include "system.h"

/**
 * @brief Finaliza o programa, liberando recursos alocados e destruindo o 
 * sistema de vacinação.
 * 
 * @param vaccinationSystem Sistema de vacinação que será destruído.
 * @param input Ponteiro para a string de entrada que será liberado.
 * @param error Código de erro que será retornado ao sistema operacional 
 * ao final da execução.
 */
void endProgram(VaccinationSystem* vaccinationSystem, char* input, int error) {
	if (input != NULL) free(input);
	destroyVaccinationSystem(vaccinationSystem);
	exit(error);
}

/**
 * @brief Finaliza o programa em caso de erro de memória, liberando recursos e 
 * exibindo uma mensagem de erro.
 * 
 * @param vaccinationSystem Sistema de vacinação que será destruído.
 * @param input Ponteiro para a string de entrada que será liberada.
 * @param pt Indicador de idioma (1 para português, 0 para outro idioma).
 */
void endProgramMemError(VaccinationSystem* vaccinationSystem, char* input, 
	int pt) {
	printError(ENOMEMORY, ENOMEMORYPT, pt);
	endProgram(vaccinationSystem, input, 1);
}

/**
 * @brief Libera a memória alocada para a data, nome do lote e nome da vacina.
 * @param date Ponteiro para a data que será liberada.
 * @param batch Ponteiro para o nome do lote que será liberado.
 * @param name Ponteiro para o nome da vacina que será liberado.
 */
void freeDateBatchName(Date date, char* batch, char* name) {
	if (date != NULL) free(date);
	if (batch != NULL) free(batch);
	if (name != NULL) free(name);
}

/**
 * @brief Libera a memória e encerra o programa em caso de erro de alocação 
 * de memória na data, no nome do lote ou da vacina.
 * 
 * @param vaccinationSystem Sistema de vacinação, utilizado para liberar 
 * recursos e encerrar o programa.
 * @param date Ponteiro para a data que foi alocada e que precisa ser liberada.
 * @param batch Ponteiro para o nome do lote que foi alocado e 
 * precisa ser liberado.
 * @param name Ponteiro para o nome da vacina que foi alocado e 
 * precisa ser liberado.
 * @param input Entrada que causou o erro.
 * @param pt Indicador de linguagem.
 */
void dateBatchNameMemError(VaccinationSystem* vaccinationSystem, Date date, 
	char* batch, char* name, char* input, int pt) {
	freeDateBatchName(date, batch, name);
	endProgramMemError(vaccinationSystem, input, pt);
}

/**
 * @brief Valida os dados de entrada para a criação de um novo lote de vacina.
 * 
 * @param batch Nome do lote a ser criado.
 * @param name Nome da vacina associada ao lote.
 * @param date Data de validade do lote.
 * @param doses_number Número de doses do lote.
 * @param vs Sistema de vacinação que contém os lotes 
 * e outras informações do sistema.
 * @param num_args Número de argumentos passados na entrada 
 * (usado para validações específicas de formatação).
 * @param pt Indicador de linguagem.
 * 
 * @return Retorna 1 se todos os parâmetros forem válidos e 0 se algum deles 
 * for inválido.
 */
int validcreateBatchInput(char* batch, char* name, Date date, int doses_number,
	VaccinationSystem* vs, int num_args, int pt) {
	if (!validBatch(batch, num_args, pt) || 
	!validBatchNumber(vs->batches_ht, batch, pt) ||
	!validName(name, num_args, pt) || 
	!validDate(vs->current_date, date, pt) || 
	!validDosesNumber(doses_number, pt)) {
		freeDateBatchName(date, batch, name);
		return 0;
	}
	return 1;
}

/**
 * @brief Faz o parsing da entrada para criar um novo lote de vacina.
 * @param vaccinationSystem Sistema de vacinação utilizado para verificar o 
 * número de lotes existentes e validar os dados.
 * @param input Entrada do usuário contendo os dados para o novo lote
 * @param batch Ponteiro para armazenar o nome do lote.
 * @param name Ponteiro para armazenar o nome da vacina.
 * @param date Ponteiro para armazenar a data do lote.
 * @param doses_number Ponteiro para armazenar o número de doses.
 * @param pt Indicador de linguagem para exibição de 
 * mensagens de erro no idioma correto.
 * 
 * @return Retorna 1 se os dados forem extraídos e validados 
 * corretamente, e 0 caso contrário.
 */
int parseCreateBatchInput(VaccinationSystem* vaccinationSystem, 
	char* input, char** batch, char** name, Date* date, 
	int* doses_number, int pt) {
	int num_args;
	if (tooManyBatchesInSystem(vaccinationSystem->batches_ht)) {
		printError(ETOOMANYVACCINES, ETOOMANYVACCINESPT, pt);
		return 0;
	}
	*date = (Date) malloc(sizeof(struct Date));
	if (*date == NULL) dateBatchNameMemError(vaccinationSystem, *date,
		*batch, *name, input, pt);
	*batch = (char*) malloc(sizeof(char)*MAX_BATCH_NAME_SIZE + 1 + 1);
	if (*batch == NULL) 
		dateBatchNameMemError(vaccinationSystem, *date, *batch, *name, 
			input, pt);
	*name = (char*) malloc(sizeof(char)*MAX_VACCINE_NAME_SIZE + 1 +1);
	if (*name == NULL) 
		dateBatchNameMemError(vaccinationSystem, *date, *batch, *name, 
			input, pt);
	num_args = sscanf(input, "c %21[A-F0-9] %d-%d-%d %d %51[^\n]", *batch, 
		&(*date)->day, &(*date)->month, &(*date)->year, 
		doses_number, *name);
	if (!validcreateBatchInput(*batch, *name, *date, *doses_number, 
		vaccinationSystem, num_args, pt)) return 0;
	return 1;
}

/**
 * @brief Cria um novo lote de vacina no sistema a partir da entrada fornecida.
 * @param vaccinationSystem Sistema de vacinação utilizado para 
 * inserir o novo lote de vacina.
 * @param input Entrada fornecida pelo usuário contendo os dados para o 
 * novo lote.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 */
void createBatchInput(VaccinationSystem* vaccinationSystem, char* input, 
	int pt) {
	char *name = NULL, *batch = NULL;
	Date date;
	int doses_number;
    if (!parseCreateBatchInput(vaccinationSystem, input, &batch, &name, 
		&date, &doses_number, pt)) {
        return;
    }
	if (!insertBatchInSystem(vaccinationSystem->batches_ht, batch, date, 
		doses_number, name))
		dateBatchNameMemError(vaccinationSystem, date, batch, name, 
			input, pt);
	printf("%s\n", batch);
	free(batch);
	free(name);
}

/**
 * @brief Processa a entrada do usuário e extrai uma lista de nomes de vacinas.
 * @param input Entrada fornecida pelo usuário contendo os nomes das vacinas, 
 * separados por espaços.
 * @param count Ponteiro para a variável que armazenará o número de vacinas 
 * extraídas da entrada.
 * @param vaccinationSystem Sistema de vacinação utilizado para chamar funções 
 * de erro, caso necessário.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 * 
 * @return Um array de strings contendo os nomes das vacinas 
 * extraídos da entrada.
 */
char** parselistBatchInput(char* input, int* count, 
	VaccinationSystem* vaccinationSystem, int pt) {
	int i = 0, capacity = 10;
	size_t length;
	char **vaccinesNames, *token;
	vaccinesNames = (char**)malloc(sizeof(char *) * capacity);
	if (vaccinesNames == NULL) 
		endProgramMemError(vaccinationSystem, input, pt);
	length = strlen(input);
	if (length > 0 && input[length - 1] == '\n') input[length - 1] = '\0';
	token = strtok(input, " ");
	while (token != NULL) {
		if (i >= capacity) {
			capacity *= 2;
			vaccinesNames = (char **)realloc(vaccinesNames, 
				sizeof(char *) * capacity);
			if (vaccinesNames == NULL) 
				endProgramMemError(vaccinationSystem, input, 
					pt);
		}
		vaccinesNames[i] = token;
		token = strtok(NULL, " ");
		i++;
	}
	*count = i;
	return (char**)realloc(vaccinesNames, sizeof(char *)*i);
}

/**
 * @brief Processa a entrada do usuário para listar lotes de vacinas.
 * @param vaccinationSystem Sistema de vacinação utilizado para 
 * acessar os lotes de vacinas no sistema e realizar operações relacionadas.
 * @param input Entrada fornecida pelo usuário que contém 
 * os nomes das vacinas ou o comando para listar todos os lotes.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 */
void listBatchInput(VaccinationSystem* vaccinationSystem, 
	char* input, int pt) {
	int count = 0;
	char** vaccinesNames = parselistBatchInput(input, &count, 
		vaccinationSystem, pt); 
	if (count == 1) {
		if (listAllBatchesInSystem(vaccinationSystem->batches_ht)==0) {
			free(vaccinesNames);
			endProgramMemError(vaccinationSystem, input, pt);
		}
	} else 
		listBatchesInSystemByGivenNames(vaccinationSystem->batches_ht, 
		vaccinesNames, count, pt);
	free(vaccinesNames);
}

/**
 * @brief Libera a memória alocada para o nome da vacina e o número do lote.
 * 
 * @param name Nome da vacina alocado dinamicamente a ser liberado.
 * @param batch Número do lote alocado dinamicamente a ser liberado.
 */
void freeVaccineAndBatchName(char* name, char* batch) {
	free(name);
	free(batch);
}

/**
 * @brief Processa a entrada do comando para aplicar uma vacina.
 * @param input A entrada fornecida pelo usuário, 
 * contendo o nome do paciente e a vacina a ser aplicada.
 * @param name Ponteiro para a variável que armazenará o nome do paciente.
 * @param vaccine_name Ponteiro para a variável que armazenará o nome da vacina.
 * @param vaccinationSystem Sistema de vacinação, usado para acessar os dados 
 * do lote e da data atual.
 * @param vaccination_date Ponteiro para a variável que armazenará a 
 * data da vacinação.
 * @param batch_info Ponteiro para a variável que armazenará as informações 
 * do lote da vacina.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 * 
 * @return Retorna 1 se a análise da entrada for bem-sucedida e o lote for 
 * válido, ou 0 caso contrário.
 */
int parseApplyVaccineInput(char* input, char** name, char** vaccine_name, 
	VaccinationSystem* vaccinationSystem, Date* vaccination_date, 
	BatchInfo** batch_info, int pt) {
	int num_args;
	*name = (char*)malloc(sizeof(char) * strlen(input) + 1);
	if (*name == NULL)
		endProgramMemError(vaccinationSystem, input, pt);
	*vaccine_name = (char*)malloc(sizeof(char)*(MAX_VACCINE_NAME_SIZE+1));
	if (*vaccine_name == NULL) {
		free(*name);
		endProgramMemError(vaccinationSystem, input, pt);
	}
	num_args = sscanf(input, "a \"%[^\"]\" %50s", *name, *vaccine_name);
	if (num_args != 2)
		num_args = sscanf(input, "a %s %50[^\n]", *name, *vaccine_name);
	*batch_info = oldestExistingValidBatchByVaccineName(
		vaccinationSystem->batches_ht, *vaccine_name, 
		vaccinationSystem->current_date);
	if (*batch_info == NULL) {
		freeVaccineAndBatchName(*vaccine_name, *name);
		printError(ENOSTOCK, ENOSTOCKPT, pt);
		return 0;
	}
	*vaccination_date = copyDate(vaccinationSystem->current_date);
	if (*vaccination_date == NULL) {
		freeVaccineAndBatchName(*vaccine_name, *name);
		endProgramMemError(vaccinationSystem, input, pt);
	}
	return 1;
}

/**
 * @brief Processa a entrada do comando para aplicar uma vacina a um paciente.
 * @param vaccinationSystem O sistema de vacinação, utilizado para acessar os
 *  dados dos registros e do lote de vacinas.
 * @param input A entrada fornecida pelo usuário, contendo o nome do paciente 
 * e o nome da vacina a ser aplicada.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 */
void applyVaccineInput(VaccinationSystem* vaccinationSystem,
	char* input, int pt) {
	char *name, *vaccine_name;
	int result;
	BatchInfo* batch_info = NULL;
	Date vaccination_date;
	if (!parseApplyVaccineInput(input, &name, &vaccine_name, 
		vaccinationSystem, &vaccination_date, &batch_info, pt)) return;
	result = insertVaccinationRecord(vaccinationSystem->records_ht, name, 
		vaccine_name, batch_info->batch, vaccination_date);
	if (result == 0) {
		freeVaccineAndBatchName(vaccine_name, name);
		free(vaccination_date);
		endProgramMemError(vaccinationSystem, input, pt);
	} else if (result == 2) {
		printError(EALREADYVACCINATED, EALREADYVACCINATEDPT, pt);
		free(vaccination_date);
	}
	else {
		batch_info->applications++;
		printf("%s\n", batch_info->batch);
	}
	freeVaccineAndBatchName(vaccine_name, name);
}

/**
 * @brief Processa a entrada do comando para remover um lote de vacinas.
 * @param vaccinationSystem O sistema de vacinação, utilizado para acessar 
 * os dados dos lotes de vacinas.
 * @param input A entrada fornecida pelo usuário, contendo o identificador do 
 * lote a ser removido.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 */
void removeBatchInput(VaccinationSystem* vaccinationSystem, char* input, 
	int pt) {
	char *batch_id;
	Batches* batch;
	batch_id = (char*)malloc(sizeof(char)*MAX_BATCH_NAME_SIZE+1);
	sscanf(input, "r %20s", batch_id);
	batch = searchBatchInSystem(vaccinationSystem->batches_ht, batch_id);
	if (batch == NULL) {
		printErrorFormated(ENOSUCHBATCH, ENOSUCHBATCHPT,pt,batch_id);
		free(batch_id);
		return;
	}
	printf("%d\n", batch->batch_info->applications);
	if (batch->batch_info->applications == 0)
		removeBatchFromSystem(vaccinationSystem->batches_ht, batch_id);
	else
		batch->batch_info->doses = 0;
	free(batch_id);
}

/**
 * @brief Lida com erro de memória ao tentar deletar um registro de vacinação.
 * 
 * @param vaccinationSystem O sistema de vacinação, usado para liberar recursos 
 * e finalizar o programa.
 * @param date A data associada ao registro a ser deletado.
 * @param batch O lote de vacina associado ao registro a ser deletado.
 * @param name O nome do indivíduo associado ao registro a ser deletado.
 * @param input A entrada fornecida pelo usuário que contém o comando para 
 * deletar o registro.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 */
void deleteRecordInputMemError(VaccinationSystem* vaccinationSystem, Date date,
	char* batch, char* name, char* input, int pt) {
	freeDateBatchName(date, batch, name);
	endProgramMemError(vaccinationSystem, input, pt);
}

/**
 * @brief Processa a exclusão de um registro de vacinação para um usuário 
 * específico.
 * 
 * @param vaccinationSystem O sistema de vacinação, 
 * usado para acessar e modificar os registros de vacinação.
 * @param input A entrada fornecida pelo usuário que contém o 
 * comando de exclusão.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 * @param name O nome do usuário que será excluído do sistema.
 * 
 * @return Retorna -1 se o usuário não existir no sistema, 
 * ou o valor retornado pela função `deleteRecordVaccinationRecordsUser` 
 * caso contrário (geralmente um código de sucesso ou erro).
 */
int deleteRecordInput1Arg(VaccinationSystem* vaccinationSystem, char* input, 
	int pt, char* name) {
	int num_args;
	num_args = sscanf(input, "d \"%[^\"]\"", name);
	if (num_args == 0) sscanf(input, "d %s", name);
	if (!userExistInSystem(vaccinationSystem->records_ht, name)) {
		printErrorFormated(ENOSUCHUSER, ENOSUCHUSERPT, pt, name);
		return -1;
	}
	return deleteRecordVaccinationRecordsUser(vaccinationSystem->records_ht,
		name);
}

/**
 * @brief Processa a exclusão de um registro de vacinação para um usuário 
 * e uma data específicos.
 * @param vaccinationSystem O sistema de vacinação, utilizado para acessar 
 * e modificar os registros de vacinação.
 * @param input A entrada fornecida pelo usuário que contém o comando de 
 * exclusão.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no 
 * idioma correto.
 * @param name O nome do usuário que terá o registro de vacinação excluído.
 * @param date A data da vacinação que será excluída do sistema.
 * 
 * @return Retorna -1 em caso de erro, como o usuário não existente 
 * ou a data inválida, ou o valor retornado pela função 
 * `deleteRecordByNameAndDate` em caso de sucesso 
 * (geralmente o número de registros deletados).
 */
int deleteRecordInput2Args(VaccinationSystem* vaccinationSystem, char* input,
	int pt, char* name, Date date) {
	int num_args, deleted;
	num_args = sscanf(input, "d \"%[^\"]\" %d-%d-%d", name, &date->day, 
		&date->month, &date->year);
	if (num_args == 0) sscanf(input, "d %s %d-%d-%d", name, &date->day, 
		&date->month, &date->year);
	if (!userExistInSystem(vaccinationSystem->records_ht, name)) {
		printErrorFormated(ENOSUCHUSER, ENOSUCHUSERPT, pt, name);
		return -1;
	}
	if (!validDate(date, vaccinationSystem->current_date, pt)) return -1;
	deleted = deleteRecordByNameAndDate(vaccinationSystem->records_ht, name,
		date);
	if (deleted == -1) {
		free(name);
		free(date);
		endProgramMemError(vaccinationSystem, input, pt);
	}
	return deleted;
}

/**
 * @brief Valida a entrada para a exclusão de um registro de vacinação com
 * três argumentos: nome do usuário, nome do lote e data de vacinação.
 * @param vaccinationSystem O sistema de vacinação, utilizado para acessar
 * os registros de vacinação e lotes.
 * @param name O nome do usuário cujo registro de vacinação será deletado.
 * @param batch_name O nome do lote de vacina associado ao registro de
 * vacinação.
 * @param date A data de vacinação que será verificada.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no
 * idioma correto.
 * 
 * @return Retorna 1 se as validações forem bem-sucedidas, ou -1 se qualquer
 * validação falhar.
 */
int validDeleteRecordInput3Args(VaccinationSystem* vaccinationSystem, 
	char* name, char* batch_name, Date date, int pt) {
	if (!userExistInSystem(vaccinationSystem->records_ht, name)) {
		printErrorFormated(ENOSUCHUSER, ENOSUCHUSERPT, pt, name);
		free(batch_name);
		return -1;
	}
	if (!validDate(date, vaccinationSystem->current_date, pt)) {
		free(batch_name);
		return -1;
	}
	if (searchBatchInSystem(vaccinationSystem->batches_ht, 
		batch_name)== NULL) {
		printErrorFormated(ENOSUCHBATCH, ENOSUCHBATCHPT, pt, 
			batch_name);
		free(batch_name);
		return -1;
	}
	return 1;
}

/**
 * @brief Processa a exclusão de um registro de vacinação com três argumentos:
 * nome do usuário, data de vacinação e nome do lote de vacina.
 * 
 * @param vaccinationSystem O sistema de vacinação, utilizado para acessar
 * os registros e lotes de vacinação.
 * @param input A entrada do usuário que contém as informações do registro
 * a ser excluído.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no
 * idioma correto.
 * @param name O nome do usuário cujo registro de vacinação será deletado.
 * @param date A data de vacinação a ser verificada.
 * 
 * @return Retorna o número de registros deletados ou -1 se ocorrer algum erro.
 */
int deleteRecordInput3Args(VaccinationSystem* vaccinationSystem,
	char* input, 
	int pt, char* name, Date date) {
	char *batch_name;
	int num_args, deleted;
	batch_name = (char*)malloc(sizeof(char)*MAX_BATCH_NAME_SIZE);
	if (batch_name == NULL) dateBatchNameMemError(vaccinationSystem, 
		date, name, batch_name, input, pt);
	num_args = sscanf(input, "d \"%[^\"]\" %d-%d-%d %20s", name, &date->day,
		&date->month, &date->year, batch_name);
	if (num_args == 0) sscanf(input, "d %s %d-%d-%d %20s", name, &date->day,
		&date->month, &date->year, batch_name);
	if (validDeleteRecordInput3Args(vaccinationSystem, name, batch_name, 
		date, pt) == -1) {
		return -1;
	}
	deleted = deleteRecordByNameDateAndBatchID(
		vaccinationSystem->records_ht, 
		name, date, batch_name);
	if (deleted == -1) dateBatchNameMemError(vaccinationSystem, date, 
		batch_name, name, input, pt);
	free(batch_name);
	return deleted;
}

/**
 * @brief Processa a exclusão de um ou mais registros de vacinação com base
 * no número de argumentos fornecidos na entrada.
 * 
 * @param vaccinationSystem O sistema de vacinação, utilizado para acessar
 * os registros de vacinação.
 * @param input A entrada do usuário que contém os parâmetros para exclusão
 * do registro de vacinação.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no
 * idioma correto.
 */
void deleteRecordInput(VaccinationSystem* vaccinationSystem, 
	char* input, int pt) {
	char *name;
	Date date;
	int num_args, deleted = 0;
	name = (char*)malloc(sizeof(char)*(strlen(input)+1));
	if (name == NULL) endProgramMemError(vaccinationSystem, input, pt);
	num_args = countArguments(input);
	if (num_args == 1) {
		deleted = deleteRecordInput1Arg(vaccinationSystem, input, pt,
			name);
		if (deleted != -1) printf("%d\n", deleted);
		free(name);
		return;
	}
	date = (Date)malloc(sizeof(struct Date));
	if (date == NULL) {
		free(name);
		endProgramMemError(vaccinationSystem, input, pt);
	}
	if (num_args == 2)
		deleted = deleteRecordInput2Args(vaccinationSystem, input, pt,
			name, date);
	else if (num_args == 3)
		deleted = deleteRecordInput3Args(vaccinationSystem, input, pt,
			 name, date);
	if (deleted != -1) printf("%d\n", deleted);
	free(date);
	free(name);
}

/**
 * @brief Exibe os registros de vacinação de um usuário específico ou de todos
 * os usuários, dependendo da entrada.
 * 
 * @param vaccinationSystem O sistema de vacinação, utilizado para acessar
 * os registros de vacinação.
 * @param input A entrada do usuário contendo os parâmetros para listagem
 * dos registros.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no
 * idioma correto.
 */
void listRecordsInput(VaccinationSystem* vaccinationSystem, char* input, 
	int pt) {
	int num_args;
	char *name;
	name = (char*)malloc(sizeof(char)*(strlen(input)+1));
	if (name == NULL) endProgramMemError(vaccinationSystem,input,pt);
	num_args = sscanf(input, "u \"%[^\"]\"", name);
	if (num_args == 0) num_args = sscanf(input, "u %s", name);
	if (num_args < 0) {
		free(name);
		if (listAllRecordsInSystem(vaccinationSystem->records_ht)==0)
			endProgramMemError(vaccinationSystem, input, pt);
		return;
	}
	if (!userExistInSystem(vaccinationSystem->records_ht, name)) {
		printErrorFormated(ENOSUCHUSER, ENOSUCHUSERPT, pt, name);
		free(name);
		return;
	}
	if (name == NULL) endProgramMemError(vaccinationSystem,input,pt);
	listAllUserRecordsInSystem(vaccinationSystem->records_ht, name);
	free(name);
}

/**
 * @brief Atualiza a data atual do sistema de vacinação com a data fornecida.
 * 
 * @param vaccinationSystem O sistema de vacinação, utilizado para alterar
 * a data atual do sistema.
 * @param input A entrada do usuário contendo a nova data.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no
 * idioma correto.
 */
void passTimeInput(VaccinationSystem* vaccinationSystem, 
	char* input, int pt) {
	Date date;
	date = (Date) malloc(sizeof(struct Date));
	if (date == NULL) endProgramMemError(vaccinationSystem,input,pt);
	sscanf(input, "t %d-%d-%d", &date->day,&date->month,&date->year);
	if (!validDate(vaccinationSystem->current_date, date, pt)) {
		free(date);
		return;
	}
	printf("%02d-%02d-%04d\n", date->day, date->month, date->year);
	free(vaccinationSystem->current_date);
	vaccinationSystem->current_date = date;
}

/**
 * @brief Processa a entrada do usuário e chama a função correspondente
 * com base no comando.
 * 
 * @param vaccinationSystem O sistema de vacinação para gerenciar as
 * operações relacionadas aos lotes e registros.
 * @param input A entrada do usuário contendo o comando e os dados.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no
 * idioma correto.
 */
void handleInputSwitch(VaccinationSystem* vaccinationSystem, char* input, 
	int pt) {
	switch (input[0]) {
		case 'q': 
			endProgram(vaccinationSystem, input, 0);
			break;
		case 'c': 
			createBatchInput(vaccinationSystem, input, pt);
			break;
		case 'l':
			listBatchInput(vaccinationSystem, input, pt);
			break;
		case 'a':
			applyVaccineInput(vaccinationSystem, input,pt);
			break;
		case 'r':
			removeBatchInput(vaccinationSystem, input, pt);
			break;
		case 'd':
			deleteRecordInput(vaccinationSystem, input,pt);
			break;
		case 'u':
			listRecordsInput(vaccinationSystem, input, pt);
			break;
		case 't':
			passTimeInput(vaccinationSystem, input, pt);
			break;
		default: break;
	}
}

/**
 * @brief Lê a entrada do usuário em loop e processa os comandos recebidos.
 * 
 * @param vaccinationSystem O sistema de vacinação para gerenciar as
 * operações relacionadas aos lotes e registros.
 * @param pt Indicador de linguagem para exibição de mensagens de erro no
 * idioma correto.
 */
void handleInput(VaccinationSystem* vaccinationSystem, int pt) {
	char* input = NULL;
	while (1) {
		input = (char*)malloc(sizeof(char)*BUFFER_SIZE + 1);
		if (input == NULL) 
			endProgramMemError(vaccinationSystem, input, pt);
		fgets(input, BUFFER_SIZE, stdin);
		handleInputSwitch(vaccinationSystem, input, pt);
		free(input);
	}
}


/**
 * @brief Função principal que inicializa o sistema de vacinação, processa
 * a entrada do usuário e gerencia a execução do programa.
 * 
 * @param argc O número de argumentos passados para o programa a partir da linha
 * de comando. Espera-se que o programa receba um argumento que indique o idioma
 * para exibição de mensagens de erro (ex: "pt" para português).
 * @param argv Os argumentos passados para o programa a partir da linha de 
 * comando. Espera-se que o argumento seja "pt" para exibir mensagens de erro em
 * português. Caso contrário, o idioma será o padrão (inglês).
 * 
 * @return Retorna 0 em caso de sucesso ou 1 em caso de erro.
 */
int main(int argc, char* argv[]) {
	int pt = 0;
	VaccinationSystem* vaccinationSystem = NULL;
	if (argc == 2 && strcmp(argv[1], PT_LANG_ARGUMENT) == 0) pt = 1;
	vaccinationSystem = initVaccinationSystem();
	if (vaccinationSystem == NULL) {
		printError(ENOMEMORY, ENOMEMORYPT, pt);
		return 1;
	}
	handleInput(vaccinationSystem, pt);
	destroyVaccinationSystem(vaccinationSystem);
	return 0;
}

/*
 * Bed_Functions.h
 *
 * Created: 21.02.2016 16:44:04
 *  Author: Christof
 */


#ifndef BED_FUNCTIONS_H_
#define BED_FUNCTIONS_H_

#include "Communication.h"
#include "ComReceiver.h"



#define NODE			'B'

#define FUNCTION_RANDOM	'R'
#define JOB_NEW_RANDOM	'N'
#define JOB_GET_RANDOM	'G'

#define FUNCTION_ZUGANG	'Z'
#define JOB_TRY_CODE	'C'

#define FUNCTION_SYSTEM		'S'
#define JOB_BL_ATTENTION	'A'
#define JOB_BL_START		'B'
#define JOB_RESET_ACT		'r'
#define JOB_RESET			'R'
#define JOB_DATE			'D'
#define JOB_TIME			'T'

#define FUNCTION_BOOT	'B'
#define DO_BOOTLOAD 	's'

#define JOB_ERROR		'!'

#define TRY_CODE_LENGTH		38

#define SECTOR_INFO		1
#define SUB_INFO		0

#define SECTOR_CARDNUM	15
#define	SUB_CARDNUM		1

#define CARD_STANDARD	1
#define CARD_SPECIAL	0

//enum {ERROR_SPEICHER=0,ERROR_PARAMETER,ERROR_NO_SMS,ERROR_HANDY,ERROR_COM_HANDY,ERROR_JOB,ERROR_TRANSMISSION};
//enum{ RCST_WAIT=0,RCST_ATTENTION,RCST_WAIT_NODE,RCST_WAIT_FUNCTION,RCST_WAIT_JOB,RCST_DO_JOB,RCST_WAIT_END1,RCST_WAIT_END2,RCST_GET_PARAMETER};

void setStatusLichtKlein();
void jobDoorIsOpen(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetCompilationDate(ComReceiver *output, char function,char address,char job, void * pMem);
void jobGetCompilationTime(ComReceiver *output, char function,char address,char job, void * pMem);
void jobSetSecurityKey(ComReceiver *output, char function,char address,char job, void * pMem);
void jobGetSecurityKey(ComReceiver *output, char function,char address,char job, void * pMem);
void jobGetFreeMemory(ComReceiver *output, char function,char address,char job, void * pMem);
void jobReceiveRandom(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobGetBlockingStatus(ComReceiver *comRec, char function,char address,char job, void * pMem);

void jobGotExternalTemperature(ComReceiver *output, char function,char address,char job, void * pMem);

void startBootloader(ComReceiver *comRec, char function,char address,char job, void * pMem);
void setBootloaderAttention(ComReceiver *comRec, char function,char address,char job, void * pMem);

/*
void rec_KNET();
void rec_state_machine_KNET(void);
void free_parameter_KNET(void);
void send_answer(uint8_t file, const char *answer, uint8_t no_error);
void send_command(uint8_t file, const char *command);
*/
uint8_t get_card_number();
uint8_t clear_card();
uint8_t try_scode(char *code);
uint8_t try_card(uint8_t cardnum);
uint8_t test_answer(uint8_t file);
uint8_t make_new_card(uint8_t cardnum);
uint8_t make_new_zero_card();
uint8_t prog_card(uint8_t cardnum);
void make_blocking(void);
uint8_t getAddress(char adr);

#endif /* BED_FUNCTIONS_H_ */

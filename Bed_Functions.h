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


void gotNewLedStatus();
void jobgotCardKey(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobReceiveRandom(ComReceiver *comRec, char function,char address,char job, void * pMem);

#endif /* BED_FUNCTIONS_H_ */

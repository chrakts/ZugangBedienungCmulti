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

#define TRY_CODE_LENGTH		38

#define SECTOR_INFO		1
#define SUB_INFO		0

#define SECTOR_CARDNUM	15
#define	SUB_CARDNUM		1

#define CARD_STANDARD	1
#define CARD_SPECIAL	0


void jobWriteCard(ComReceiver *comRec, char function,char address,char job, void * pMem);
void gotNewLedStatus();
void jobReleaseCard(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobgotCardKey(ComReceiver *comRec, char function,char address,char job, void * pMem);
void jobReceiveRandom(ComReceiver *comRec, char function,char address,char job, void * pMem);

#endif /* BED_FUNCTIONS_H_ */

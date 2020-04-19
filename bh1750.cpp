/*
 * CPPFile1.cpp
 *
 * Created: 01.01.2016 13:31:08
 *  Author: Christof
 */ 

#include "Bedienung.h"

BH1750::BH1750(void)
{
}

BH1750::BH1750(TWI_Master_t *mytwi,int address)
{
	twi = mytwi;
	twi_address = address;
	Set_Mode(BH1750_Cont_H_Res);
}

uint16_t BH1750::Get_Data()
{
	TWI_MasterRead(twi,twi_address,2);
	
	while (twi->status != TWIM_STATUS_READY) {
		/* Wait until transaction is complete. */
	}
	return( (uint16_t)(((twi->readData[0])<<8)|(twi->readData[1])) );
}

void BH1750::Set_Mode(uint8_t mode)
{
uint8_t md;
	md = mode;
	TWI_MasterWrite(twi,twi_address,&md,1);
	
	while (twi->status != TWIM_STATUS_READY) {
		/* Wait until transaction is complete. */
	}
}

void BH1750::PowerOn()
{
uint8_t md;
	md = BH1750_Power_On;
	TWI_MasterWrite(twi,twi_address,&md,1);

	while (twi->status != TWIM_STATUS_READY) {
		/* Wait until transaction is complete. */
	}
}

void BH1750::PowerOff()
{
	uint8_t md;
	md = BH1750_Power_Down;
	TWI_MasterWrite(twi,twi_address,&md,1);

	while (twi->status != TWIM_STATUS_READY) {
		/* Wait until transaction is complete. */
	}
}

void BH1750::ResetData()
{
	uint8_t md;
	md = BH1750_Reset_Data;
	TWI_MasterWrite(twi,twi_address,&md,1);

	while (twi->status != TWIM_STATUS_READY) {
		/* Wait until transaction is complete. */
	}
}

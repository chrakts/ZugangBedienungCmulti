#include "Bedienung.h"

#define TSteigung 0.147892720306513
#define TOffset   -294.872030651341

volatile uint16_t temperatureWert;
volatile uint16_t tempCalib85;
volatile uint8_t listPointer=0;
volatile uint8_t adcMode=ADCMODE_CF_NORMAL;

void initStromMessung(bool doOffset)
{
  listPointer = 0;
  ADCA.CTRLB     = ADC_CONMODE_bm;// | ADC_FREERUN_bm;
  ADCA.REFCTRL   = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm | ADC_TEMPREF_bm;
  ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
  ADCA.CH0.CTRL    = ADC_CH_GAIN_16X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
  if(doOffset)
    ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN7_gc | ADC_CH_MUXNEG_PIN7_gc;
  else
    ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN6_gc | ADC_CH_MUXNEG_PIN7_gc;
  ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc;
  ADCA.CALL = SP_ReadCalibrationByte( PRODSIGNATURES_ADCACAL0 );
	ADCA.CALH = SP_ReadCalibrationByte( PRODSIGNATURES_ADCACAL1 );
  ADCA.CTRLA     = ADC_ENABLE_bm;
}

void initTemperatureMessung()
{
  listPointer = 0;
  ADCA.CTRLB     = 0; // ADC_CONMODE_bm;
  ADCA.REFCTRL   = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm | ADC_TEMPREF_bm;
  ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
  ADCA.CH0.CTRL    = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_INTERNAL_gc;
  ADCA.CH0.MUXCTRL = 0;
  ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc;
  cli();
  ADCA.CALL = SP_ReadCalibrationByte( 0x0020 );
  _delay_ms(10);
	ADCA.CALH = SP_ReadCalibrationByte( 0x0021 );
  _delay_ms(10);
  //tempCalib85 = ((uint16_t)(SP_ReadCalibrationByte( PRODSIGNATURES_TEMPSENSE1 )))<<8 ;
  //tempCalib85 = ((uint16_t)(SP_ReadCalibrationByte( 0x002F )))<<8 ;
  _delay_ms(10);
  //tempCalib85 += (uint16_t)SP_ReadCalibrationByte( PRODSIGNATURES_TEMPSENSE0 )
  //tempCalib85 += (uint16_t)SP_ReadCalibrationByte( 0x002E );
  _delay_ms(10);
	sei();
  ADCA.CTRLA     = ADC_ENABLE_bm;
}

void startAdcMeasure()
{
  ADCA.CH0.CTRL    |= ADC_CH_START_bm;
}

uint16_t readStrom()
{
int16_t result;
  result = ADCA.CH0.RES;
  return(abs(result));
}

void waitMeasure()
{

}

double calcTemperatur()
{
  return( TSteigung*(double)temperatureWert + TOffset);
}

uint8_t SP_ReadCalibrationByte( uint8_t index )
{
	uint8_t result;

	/* Load the NVM Command register to read the calibration row. */
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	result = pgm_read_byte(index);

	/* Clean up NVM Command register. */
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;

	return result;
}

SIGNAL(ADCA_CH0_vect)
{
#define ANZAHL_WERTE 16


static uint16_t tempList[ANZAHL_WERTE];
uint16_t result;

  tempList[listPointer] = readStrom();
  listPointer++;
  if(listPointer>=ANZAHL_WERTE)
  {
    result = 0;
    for(uint8_t i=0;i<ANZAHL_WERTE;i++)
      result += tempList[i];
    temperatureWert = (uint16_t)(result >> 4);
    listPointer=0;
  }
}


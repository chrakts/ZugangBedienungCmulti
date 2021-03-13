#ifndef RFCARDS_H_INCLUDED
#define RFCARDS_H_INCLUDED

#define KEY_NUM	 10
#define INFO_NUM 10

#define KEY_LENGTH 6
#define INFO_LENGTH 6
#define SCODE_LENGTH	6

extern volatile uint8_t cardStatus,cardNumber;
extern volatile uint8_t cardKey[KEY_LENGTH],cardInfo[INFO_LENGTH];

enum{CARD_UNSELECTED,CARD_SELECTED,CARD_WAITING_FOR_WRITE,CARD_WAITING_FOR_LOCK,CARD_WAITING_FOR_CLEAR};

uint8_t writeNewCard(volatile uint8_t cardnum,volatile uint8_t* key,volatile uint8_t* info);
uint8_t clearCard(volatile uint8_t cardnum,volatile uint8_t* oldkey);
uint8_t get_card_number();

uint8_t getAddress(char adr);

#endif // RFCARDS_H_INCLUDED

#ifndef RFCARDS_H_INCLUDED
#define RFCARDS_H_INCLUDED


uint8_t writeNewCard(uint8_t cardnum,uint8_t* keyInfo);
uint8_t get_card_number();
uint8_t clear_card();
uint8_t try_card(uint8_t cardnum);
uint8_t make_new_card(uint8_t cardnum);
uint8_t make_new_zero_card();
uint8_t prog_card(uint8_t cardnum);
uint8_t getAddress(char adr);

#endif // RFCARDS_H_INCLUDED

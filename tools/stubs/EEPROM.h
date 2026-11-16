#pragma once
struct EEPROMC{ template<class T> void get(int,T&){} template<class T> void put(int,const T&){} }; extern EEPROMC EEPROM;
#define PROGMEM
#define pgm_read_byte(p) (*(p))

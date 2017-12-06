#ifndef DDT_DISPLAY_H_
#define DDT_DISPLAY_H_

#include <avr/eeprom.h>
#include "lcd/lcd.h"

void lcd_display_string(const char *);
void lcd_print_character(const uint8_t);
void display_print_uint32(uint32_t);
void display_print_uint8(uint8_t);

#endif

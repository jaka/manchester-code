#ifndef _LCD16_H_
#define _LCD16_H_

#include <avr/io.h>

#define LCD16_ENABLE_PORT	PORTD
#define LCD16_ENABLE_DDR	DDRD
#define LCD16_ENABLE_PIN	PD2

#define LCD16_RW_PORT	PORTD
#define LCD16_RW_DDR	DDRD
#define LCD16_RW_PIN	PD3

#define LCD16_REGISTER_PORT	PORTD
#define LCD16_REGISTER_DDR	DDRD
#define LCD16_REGISTER_PIN	PD4

void lcd_setup(void);
void lcd_init(void);
void lcd_clean(void);
void lcd_goto(const uint8_t, const uint8_t);
void lcd_load_character(const uint8_t, const uint8_t *);
void lcd_write_character_4d(const uint8_t);
uint8_t lcd_read(void);

#endif

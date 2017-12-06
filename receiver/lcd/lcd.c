#include "lcd.h"

#include <util/delay.h>

/* Initialization */
#define _lcd_RESET	0x30
#define _lcd_4BIT	0x20

/* 0x01 - Clear display */
#define _lcd_CLEAR	0x01

/* 0x04 - Entry mode set */
#define _lcd_ENTRY	0x04
#define _lcd_ENTRY_ID	0x02 /* Increment */

/* 0x08 - Display on/off control */
#define _lcd_DISPLAY	0x08
#define _lcd_DISPLAY_ON	0x04

/* 0x20 - Function set */
#define _lcd_FUNCTION	0x20
#define _lcd_2LINE	0x08

/* 0x40 - Set CGRAM address */
#define _lcd_CGRAM	0x40

/* 0x80 - Set DDRAM address */
#define _lcd_DGRAM	0x80

static const uint8_t _lcd16_row[2] = { 0x00, 0x40 };

static void _lcd16_write_4_low(const uint8_t b)
{
  uint8_t r;

  r = 0;
  if (b & 0x10)
    r |= 0x8;
  if (b & 0x20)
    r |= 0x4;
  if (b & 0x40)
    r |= 0x2;
  if (b & 0x80)
    r |= 0x1;

  DDRC |= 0x0f;
  PORTC = r | (PORTC & 0xf0);
   LCD16_ENABLE_PORT |= _BV(LCD16_ENABLE_PIN);
  _delay_us(1);
  LCD16_ENABLE_PORT &= ~_BV(LCD16_ENABLE_PIN);
  _delay_us(1);
  DDRC &= ~0x0f;
}

static uint8_t _lcd16_read_4_low(void)
{
  uint8_t b, r;

  LCD16_ENABLE_PORT |= _BV(LCD16_ENABLE_PIN);
  _delay_us(1);
  b = PINC;
  LCD16_ENABLE_PORT &= ~_BV(LCD16_ENABLE_PIN);
  _delay_us(1);

  r = 0;
  if (b & 0x1)
    r |= 0x8;
  if (b & 0x2)
    r |= 0x4;
  if (b & 0x4)
    r |= 0x2;
  if (b & 0x8)
    r |= 0x1;

  return r;
}

uint8_t lcd_read(void)
{
  uint8_t rv;
  PORTC &= 0xf0;
  LCD16_RW_PORT |= _BV(LCD16_RW_PIN);
  LCD16_REGISTER_PORT &= ~_BV(LCD16_REGISTER_PIN);
  rv = _lcd16_read_4_low() << 4;
  rv |= _lcd16_read_4_low();
  LCD16_RW_PORT &= ~_BV(LCD16_RW_PIN);
  return rv;
}

static void _lcd16_write_4(const uint8_t b)
{
  _lcd16_write_4_low(b);
}

static void _lcd16_delay(void)
{
#if 1
  uint8_t d;
  d = 16;  
  while (--d && (lcd_read() & 0x80)) {
    _delay_us(5);
  }
#else
  _delay_us(80);
#endif
}

static void lcd_write_instruction(const uint8_t b)
{
  LCD16_REGISTER_PORT &= ~_BV(LCD16_REGISTER_PIN);
  LCD16_ENABLE_PORT &= ~_BV(LCD16_ENABLE_PIN);
  _lcd16_write_4(b);
  _lcd16_write_4(b << 4);
  _lcd16_delay();
}

void lcd_write_character_4d(const uint8_t b)
{ 
  LCD16_REGISTER_PORT |= _BV(LCD16_REGISTER_PIN);
  LCD16_ENABLE_PORT &= ~_BV(LCD16_ENABLE_PIN);
  _lcd16_write_4(b);
  _lcd16_write_4(b << 4);
  _lcd16_delay();
}

void lcd_load_character(const uint8_t p, const uint8_t *data)
{
  uint8_t i;

  lcd_write_instruction(_lcd_CGRAM | ((p & 0x07) << 3));
  for (i = 0; i < 8; i++)
    lcd_write_character_4d(*(data + i));
}

void lcd_clean(void)
{
  lcd_write_instruction(_lcd_CLEAR);
  _delay_ms(4);
  lcd_write_instruction(_lcd_ENTRY | _lcd_ENTRY_ID);
}

void lcd_goto(const uint8_t x, const uint8_t y)
{
  lcd_write_instruction(x + (_lcd_DGRAM | _lcd16_row[y]));
}

void lcd_init(void)
{
  LCD16_REGISTER_PORT &= ~_BV(LCD16_REGISTER_PIN);
  LCD16_ENABLE_PORT &= ~_BV(LCD16_ENABLE_PIN);

  _lcd16_write_4(_lcd_RESET);
  _delay_ms(5);
  _lcd16_write_4(_lcd_RESET);
  _delay_us(100);
  _lcd16_write_4(_lcd_RESET);
  _delay_us(80);

  _lcd16_write_4(_lcd_4BIT);
  _delay_us(80);

  lcd_write_instruction(_lcd_FUNCTION | _lcd_2LINE); 

  lcd_clean();

  lcd_write_instruction(_lcd_DISPLAY | _lcd_DISPLAY_ON);
}

void lcd_setup(void)
{
  LCD16_RW_DDR |= _BV(LCD16_RW_PIN);
  LCD16_RW_PORT &= ~_BV(LCD16_RW_PIN);

  LCD16_ENABLE_DDR |= _BV(LCD16_ENABLE_PIN);
  LCD16_REGISTER_DDR |= _BV(LCD16_REGISTER_PIN);
}

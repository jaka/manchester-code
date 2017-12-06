#include "display.h"

void lcd_print_character(const uint8_t b)
{
  lcd_write_character_4d(b);
}

void lcd_display_string(const char *str)
{
  uint8_t i = 0;

  while (*(str + i)) {
    lcd_print_character(*(str + i));
    i++;
    if (i > 40)
      break;
  }
}

void display_print_uint8(uint8_t v) {

  uint8_t d, t;
  uint8_t s = 0;
  uint8_t j;
  uint8_t i = v;
  while (i >= 10) {
    s++;
    i /= 10;
  }
  i = v;
  while (s) {
    t = s;
    j = 1;
    while (t--)
      j *= 10;
    d = 0;
    while (i >= j) {
      d++;
      i -= j;
    }
    lcd_write_character_4d(d + '0');
    s--;
  };
  lcd_write_character_4d(i + '0');
}

void display_print_uint32(uint32_t v) {

  uint8_t d, s, t;
  uint32_t i;

  i = v;
  s = 0;
  while (i >= 10) {
    s++;
    i /= 10;
  }
  if (s < 2)
    s = 2;
  while (s) {
    t = s;
    i = 1;
    while (t--)
      i *= 10;
    d = 0;
    while (v >= i) {
      d++;
      v -= i;
    }
    if (s == 1)
      lcd_write_character_4d('.');
    lcd_write_character_4d(d + '0');
    s--;
  };
  lcd_write_character_4d(v + '0');
}

#include "display.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define GENERATOR 0x8c

static uint8_t lsb_crc8(uint8_t *data_in, uint8_t len, const uint8_t generator)
{
  uint8_t i, bit_counter;
  uint8_t crc = 0;

  for (i = 0; i < len; i++) {
    crc ^= *(data_in + i);
    bit_counter = 8;
    do {
      if (crc & 0x01)
        crc = (((crc >> 1) & 0x7f) ^ generator);
      else
        crc = (crc >> 1) & 0x7f;
      bit_counter--;
    } while (bit_counter > 0);
  }
  return crc;
}

#define PSEUDOWIRE_IN	PIND
#define PSEUDOWIRE_PIN	PD5

#define IDLE 0
#define SYNC 1
#define RECV 3
#define OK 4

#define PULSE 8
#define MAX_BYTE 4

static volatile struct pseudowire {
  uint8_t status;
  uint8_t buf[MAX_BYTE];
} me;

ISR (TIMER0_COMPA_vect)
{
  static struct pseudowire_recv {
    uint8_t bit;
    uint8_t last_rx;
    uint8_t shifter;
    uint8_t byte;
    uint8_t counter;
    uint8_t sync_counter;
  } pw;

  uint8_t rx;

  pw.counter++;

  if (pw.counter > (3 * PULSE)) {
    me.status = IDLE;
    pw.counter = 0;
  }

  rx = (PSEUDOWIRE_IN & _BV(PSEUDOWIRE_PIN)) == _BV(PSEUDOWIRE_PIN);

  if (pw.last_rx == rx)
    return;

  switch (me.status) {

    case IDLE:
      if (rx) {
        pw.sync_counter = 0;
        me.status = SYNC;
      }
      break;

    case SYNC:
      if (pw.counter > (2 * PULSE)) {
        if (pw.sync_counter < 6 || rx == 1) {
          me.status = IDLE;
          break;
        }
        pw.byte = 0;
        *me.buf = 0;
        pw.shifter = 0x80;
        me.status = RECV;
        pw.bit = 0;
      }
      pw.sync_counter++;
      break;

    case RECV:
      if (pw.counter > (2 * PULSE))
        pw.bit = !pw.bit;
      else if (pw.counter < PULSE || rx == 0)
        break;
      if (pw.bit)
        *(me.buf + pw.byte) |= pw.shifter;
      pw.shifter >>= 1;
      if (!pw.shifter) {
        pw.byte++;
        *(me.buf + pw.byte) = 0;
        pw.shifter = 0x80;
        if (pw.byte == MAX_BYTE) {
          if (lsb_crc8((uint8_t *)me.buf, MAX_BYTE - 1, GENERATOR) == me.buf[MAX_BYTE - 1]) {
            me.status = OK;
}
          else
            me.status = IDLE;
        }
      }
      break;
  }
  pw.counter = 0;
  pw.last_rx = rx;
}

void display_print_int(uint8_t i) {

  uint8_t hundreds = 0, tens = 0, ones;

  while (i >= 100) {hundreds++; i -= 100;}
  if (!hundreds)
    lcd_write_character_4d(' ');
  else
    lcd_write_character_4d(hundreds + '0');

  while (i >= 10) {tens++; i -= 10;}
  if (hundreds == 0 && tens == 0)
    lcd_write_character_4d(' ');
  else
    lcd_write_character_4d(tens + '0');

  ones = i;
  lcd_write_character_4d(ones + '0');
}

int main(void)
{

  TCCR0A = _BV(WGM01);
  TCCR0B = _BV(CS00) | _BV(CS01); /* 1/64 */

  TCNT0 = 0;
  OCR0A = 19;
  TIMSK0 = _BV(OCIE0A);
  TIFR0 = 0;
  sei();

  DDRB |= _BV(PB2);
  PORTB &= ~_BV(PB2);
  lcd_setup();

  _delay_ms(10);

  lcd_init();

  if (!(PINC & _BV(PC5)))
    lcd_display_string("S!");

  for (;;) {
    sleep_mode();

  if (me.status == OK) {
    uint8_t i;
    for (i = 0; i < MAX_BYTE; i++) {
      lcd_goto(4 * i, 1);
      display_print_uint8(me.buf[i]); 
      lcd_write_character_4d(' ');
    }
  }
    
  }

}

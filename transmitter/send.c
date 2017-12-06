#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

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

uint8_t buf[4] = { 0xaa, 0x00, 0x01, 0x02 };

#define INIT 0
#define SYNC 1
#define SEND 2
#define RECV 3
#define OK 4
#define END 9

#define MAX_BYTE 4

#define PSEUDOWIRE_PORT	PORTD
#define PSEUDOWIRE_PIN	PD4

static volatile struct {
  uint8_t status;
  uint8_t shifter;
  uint8_t byte;
} me;

ISR (TIMER2_COMPA_vect)
{
  static uint8_t state = 0, bit;

  if (state == 2) { /* first pulse */
    if (bit)
      PSEUDOWIRE_PORT &= ~_BV(PSEUDOWIRE_PIN);
    else
      PSEUDOWIRE_PORT |= _BV(PSEUDOWIRE_PIN);
    state = 1;
  }
  else {
    if (state == 1) { /* second pulse */
      if (bit)
        PSEUDOWIRE_PORT |= _BV(PSEUDOWIRE_PIN);
      else
        PSEUDOWIRE_PORT &= ~_BV(PSEUDOWIRE_PIN);
      state = 0;
    }

    switch (me.status) {

    case INIT:
      me.shifter = 0x80;
      me.status = SYNC;
      return;

    case SYNC:
      bit = 1;
      state = 2;
      me.shifter >>= 1;
      if (!me.shifter) {
        bit = 0;
        me.byte = 0;
        me.shifter = 0x80;
        me.status = SEND;
      }
      break;

    case SEND:
      bit = 0;
      state = 2;
      if (buf[me.byte] & me.shifter)
        bit = 1;
      me.shifter >>= 1;
      if (!me.shifter) { 
        me.byte++;
        me.shifter = 0x80;
        if (me.byte == MAX_BYTE)
          me.status = END;
      }
      break;

    case END:
      bit = 0;
      state = 2;
      me.status = OK;
      break;

    case OK:
      state = 0;
      break;

    }
  }
}

int main(void)
{

  DDRD = _BV(PD5) | _BV(PSEUDOWIRE_PIN);

  TCCR2A = _BV(WGM21);
  TIMSK2 = _BV(OCIE2A);
  TIFR2 = 0;
  TCNT2 = 0;
  OCR2A = (128 - 1);
  TCCR2B = _BV(CS21); /* 1/8 prescaller. */

  sei();

  me.status = OK;

  for (;;) {

    if (me.status == OK) {
      buf[1]++;
      buf[3] = lsb_crc8(buf, 3, GENERATOR);
      _delay_ms(30);
      me.status = INIT;
      DDRD ^= _BV(PD5);

    }

  }
  return 0;
}

#!/usr/bin/make -f

OPTS	= -Os -mmcu=$(MCU_TARGET) -DF_CPU=$(MCU_FREQ)
CFLAGS	= -Wall -Werror -Wextra -pedantic -std=c99 -ffunction-sections -fdata-sections -finline-functions
LDFLAGS	= -Wl,--gc-sections
LIBS	=

CC := avr-gcc
OBJCOPY := avr-objcopy

MCU_FREQ := 11059200UL
MCU_TARGET := atmega328p
PROGRAMMER_MCU := m328p
PROGRAMMER := usbasp

SRC := recv.c lcd/lcd.c display.c
OBJ := $(patsubst %.c,%.o,$(SRC))
NAME = $(MCU_TARGET)-manchester

all: $(NAME).hex

%.o: %.c
	$(CC) $(CFLAGS) $(OPTS) -c -o $@ $<

$(NAME).elf: $(OBJ)
	$(CC) $(LDFLAGS) $(OPTS) -o $@ $^ $(LIBS)

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.epp: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

clean:
	rm -f $(OBJ)
	rm -f $(NAME).elf
	rm -f $(NAME).hex
	rm -f $(NAME).epp

fuse:
	avrdude -c $(PROGRAMMER) -p $(PROGRAMMER_MCU) -B 5 -U lfuse:w:0xff:m -U hfuse:w:0xd9:m

flash: $(NAME).hex
	avrdude -c $(PROGRAMMER) -p $(PROGRAMMER_MCU) -U flash:w:$(NAME).hex

slow-flash: $(NAME).hex $(NAME).epp
	avrdude -c $(PROGRAMMER) -p $(PROGRAMMER_MCU) -B 5 -U flash:w:$(NAME).hex -U eeprom:w:$(NAME).epp

size: $(NAME).elf
	avr-size -C --mcu=$(MCU_TARGET) $<

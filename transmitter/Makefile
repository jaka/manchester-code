#!/usr/bin/make -f

OPTS	= -Os -mmcu=$(MCU_TARGET) -DF_CPU=$(MCU_FREQ)
CFLAGS	= -Wall -Werror -Wextra -pedantic -std=c99 -ffunction-sections -fdata-sections -finline-functions
LDFLAGS	= -Wl,--gc-sections
LIBS	=

CC := avr-gcc
OBJCOPY := avr-objcopy

MCU_FREQ := 1000000UL
MCU_TARGET := atmega328p
PROGRAMMER_MCU := m328p
PROGRAMMER := usbasp

SRC := send.c
OBJ := $(patsubst %.c,%.o,$(SRC))
NAME = $(MCU_TARGET)-manchester

all: build

build: $(NAME).hex

clean:
	rm -f $(OBJ)
	rm -f $(NAME).elf
	rm -f $(NAME).hex

flash: $(NAME).hex
	avrdude -c $(PROGRAMMER) -p $(PROGRAMMER_MCU) -B 3 -U flash:w:$<

$(NAME).elf: $(OBJ)
	$(CC) $(CFLAGS) $(OPTS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.o: %.c
	$(CC) $(CFLAGS) $(OPTS) -c -o $@ $<

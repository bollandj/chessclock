/*
 * display.h
 *
 * Created: 04/07/2020 13:46:35
 *  Author: Joshua
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define SPI_DDR  DDRB
#define SPI_PORT PORTB

#define SCK  PB5
#define MOSI PB3
#define CS   PB2

extern volatile uint8_t displayBuffer[8];

extern volatile uint8_t blinkOsc[8];
extern volatile uint8_t blinkMask[8];

void init_display(void);
void do_blink(void);


#endif /* DISPLAY_H_ */
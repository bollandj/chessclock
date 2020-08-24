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
#define SS   PB2
#define CS   PB0

inline void COLON_ON()  {PORTD |= 1<<PD4;}
inline void COLON_OFF() {PORTD &= ~(1<<PD4);}

extern uint8_t hmms;

extern volatile uint8_t displayBuffer[8];

extern volatile uint8_t blinkOsc[8];
extern volatile uint8_t blinkMask[8];

extern uint8_t blinkTimer;
extern const uint8_t blinkThreshold;

extern const uint8_t sevenSeg[38];

void init_display(uint8_t brightness);
void update_blink(void);

void write_time(uint8_t edit);
void write_char(uint8_t chr, uint8_t pos);
void write_string(const char *str, uint8_t start, uint8_t end);
void write_number_8(uint8_t num, uint8_t start);


#endif /* DISPLAY_H_ */
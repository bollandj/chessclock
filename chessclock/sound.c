/*
 * sound.c
 *
 * Created: 11/07/2020 13:08:17
 *  Author: Joshua
 */ 

#include "sound.h"

const uint8_t dutyCycle=31;

void init_sound(void)
{
	DDRB |= 1<<PB1 | 1<<PB2;
	
	TCCR1A = 1<<COM1A1 | 1<<COM1B1 | 1<<COM1B0 | 1<<WGM10;	
	OCR1A = dutyCycle;
	OCR1B = dutyCycle;
}

void sound_on(void)
{
	TCCR1B = 1<<WGM12 | 1<<CS11 | 1<<CS10;	
}

void sound_off(void)
{
	TCCR1B = 0x00;
}

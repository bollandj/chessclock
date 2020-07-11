/*
 * sound.c
 *
 * Created: 11/07/2020 13:08:17
 *  Author: Joshua
 */ 

#include "sound.h"

void init_sound(void)
{
	DDRB |= 1<<PB1; // OC1A
	
	TCCR1A = 1<<COM1A0 | 1<<WGM11 | 1<<WGM10; // Fast PWM, toggle, TOP = 0CR1A
	//TCCR1B = 1<<WGM13 | 1<<WGM12 | 1<<CS11; // /8
	OCR1A = 999;
}

void sound_on(void)
{
	TCCR1B = 1<<WGM13 | 1<<WGM12 | 1<<CS11;	
}

void sound_off(void)
{
	TCCR1B = 0x00;
}

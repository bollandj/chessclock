/*
 * sound.c
 *
 * Created: 11/07/2020 13:08:17
 *  Author: Joshua
 */ 

#include "sound.h"

void init_sound(void)
{
	DDRD |= 1<<PD5;
}

void sound_on(void)
{
	TCCR1B = 1<<WGM13 | 1<<WGM12 | 1<<CS11;	
}

void sound_off(void)
{
	TCCR1B = 0x00;
}

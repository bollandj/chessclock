/*
 * sound.c
 *
 * Created: 11/07/2020 13:08:17
 *  Author: Joshua
 */ 

#include "sound.h"

uint8_t beepTimer;
const uint8_t beepTime=3;
const uint16_t beepFreq=1024;

void init_sound(void)
{
	DDRB |= 1<<PB1 | 1<<PB2; // OC1A, OC1B
	
	TCCR1A = 1<<COM1A1 | 1<<COM1B1 | 1<<COM1B0 | 1<<WGM11; // fast PWM, TOP = ICR1	
	TCCR1B = 1<<WGM13 | 1<<WGM12 | 1<<CS11;	               // /8 prescaler		
		
	beepTimer = 0;
}

void update_beep(void)
{	
	if (beepTimer > 0) beepTimer--;
	else               TCCR1B = 0x00;		
}

void beep(uint8_t length)
{
	ICR1 = beepFreq;               // for some reason this can't be set in the init function
	OCR1A = OCR1B = beepFreq >> 2; // 25% duty cycle
	beepTimer = beepTime << length;
	TCCR1B = 1<<WGM13 | 1<<WGM12 | 1<<CS11;
}

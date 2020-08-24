/*
 * sound.c
 *
 * Created: 11/07/2020 13:08:17
 *  Author: Joshua
 */ 

#include "sound.h"

uint8_t beepTimer;
uint16_t beepFreq;

uint16_t const notes[128] PROGMEM =
{
	65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,64792,61155,57723,54483,51425,
	48539,45814,43243,40816,38525,36363,34322,32395,30577,28861,27241,25712,24269,22907,21621,20407,
	19262,18181,17160,16197,15288,14430,13620,12855,12134,11453,10810,10203,9630,9090,8580,8098,
	7644,7214,6809,6427,6066,5726,5404,5101,4815,4544,4289,4049,3821,3607,3404,3213,
	3033,2862,2702,2550,2407,2272,2144,2024,1910,1803,1702,1606,1516,1431,1350,1275,
	1203,1135,1072,1011,955,901,850,803,757,715,675,637,601,567,535,505,
	477,450,425,401,378,357,337,318,300,283,267,252,238,224,212,200,
	189,178,168,158,149,141,133,126,118,112,105,99,94,88,83,79
};

void init_sound(void)
{
	DDRB |= 1<<PB1 | 1<<PB2; // OC1A, OC1B
	
	TCCR1A = 1<<COM1A1 | 1<<COM1B1 | 1<<COM1B0 | 1<<WGM11; // fast PWM, TOP = ICR1	
	TCCR1B = 1<<WGM13 | 1<<WGM12 | 1<<CS11;	               // /8 prescaler		
		
	beepTimer = 0;
	beepFreq = pgm_read_word(&notes[72]); 
}

void update_beep(void)
{	
	if (beepTimer > 0) beepTimer--;
	else               TCCR1B = 0x00;		
}

void beep(uint8_t length)
{
	ICR1 = beepFreq;               // for some reason this can't be set just in the init function
	OCR1A = OCR1B = beepFreq >> 2; // 25% duty cycle
	beepTimer = length;
	TCCR1B = 1<<WGM13 | 1<<WGM12 | 1<<CS11;
}

void tune()
{
	uint8_t n[8] = {60, 62, 64, 65, 67, 69, 71, 72};
	uint8_t d[8] = {250, 250, 250, 250, 250, 250, 250, 250};
	
	for (uint8_t i = 0; i < 8; i++)
	{
		ICR1 = pgm_read_word(&notes[n[i]]);              
		OCR1A = OCR1B = ICR1 >> 2;
		
		TCCR1B = 1<<WGM13 | 1<<WGM12 | 1<<CS11;
		for (uint8_t j = 0; j < d[i]; j++) _delay_ms(1);
		TCCR1B = 0x00;
	}
}

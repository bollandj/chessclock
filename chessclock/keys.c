/*
 * keys.c
 *
 * Created: 04/07/2020 16:27:32
 *  Author: Joshua
 */ 

#include "keys.h"

uint8_t holdTimer;
const uint8_t holdTimerThreshold=60;

void init_keys(void)
{
	EICRA = 1<<ISC11 | 1<<ISC01; // falling edge
	EIMSK = 1<<INT1 | 1<<INT0;   // enable INT0, INT1
	PORTD |= 1<<PD2 | 1<<PD3;    // enable pullups
	
	KEY_PORT |= KEY_MASK; // enable pullups
	
	scan_keys(); scan_keys(); // avoid detecting keypresses on startup
}

void scan_keys(void)
{
	lastKeyState = keyState;
		
	keyState = KEY_PIN;
	uint8_t keyDiff = keyState ^ lastKeyState;
	
	keyPressed = keyDiff & ~keyState;	
	keyReleased = keyDiff & keyState;
	
	if (~keyState & START_KEY)
	{
		if (holdTimer < holdTimerThreshold) holdTimer++;		
	}
	else
	{
		holdTimer = 0;
	}

}

/*
 * sound.h
 *
 * Created: 11/07/2020 13:08:08
 *  Author: Joshua
 */ 

#define F_CPU 8000000UL

#ifndef SOUND_H_
#define SOUND_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

extern uint8_t beepTimer;
extern uint16_t beepFreq;

extern const uint16_t notes[128] PROGMEM;

void init_sound(void);

void update_beep(void);

void beep(uint8_t length);

void tune();

#endif /* SOUND_H_ */
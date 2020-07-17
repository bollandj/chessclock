/*
 * sound.h
 *
 * Created: 11/07/2020 13:08:08
 *  Author: Joshua
 */ 


#ifndef SOUND_H_
#define SOUND_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

extern uint8_t beepTimer;
extern const uint8_t beepTime;
extern const uint16_t freq;

void init_sound(void);

void update_beep(void);

void beep(uint8_t length);

#endif /* SOUND_H_ */
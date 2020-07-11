/*
 * sound.h
 *
 * Created: 11/07/2020 13:08:08
 *  Author: Joshua
 */ 


#ifndef SOUND_H_
#define SOUND_H_

#include <avr/io.h>

void init_sound(void);

void sound_on(void);
void sound_off(void);

#endif /* SOUND_H_ */
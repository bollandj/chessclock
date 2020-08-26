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

extern const uint16_t scale[12] PROGMEM;

extern const uint8_t abc[270] PROGMEM;

extern uint16_t abcPtr;
extern uint8_t unitNoteLength;
extern uint8_t key;

extern uint8_t duration;
extern uint16_t divisor;

#define IS_DIGIT(x) (((x) >= '0') && ((x) <= '9'))
#define IS_ALPHA(x) ((((x) >= 'A') && ((x) <= 'Z')) || (((x) >= 'a') && ((x) <= 'z')))
#define IS_UPPER(x) (((x) >= 'A') && ((x) <= 'Z'))
#define IS_LOWER(x) (((x) >= 'a') && ((x) <= 'z'))

extern uint8_t int_log(uint8_t n);
extern int8_t skip_to(char t);
extern char get_char();
extern uint8_t get_int();

extern void read_header();
extern void read_notes();

extern void init_sound(void);
extern void update_beep(void);
extern void beep(uint8_t length);
extern void tune();

#endif /* SOUND_H_ */
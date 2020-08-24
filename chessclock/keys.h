/*
 * keys.h
 *
 * Created: 04/07/2020 16:27:25
 *  Author: Joshua
 */ 


#ifndef KEYS_H_
#define KEYS_H_

#include <avr/io.h>

#define KEY_PORT PORTC
#define KEY_PIN  PINC

#define MODE_KEY  1<<PC4
#define TIME_KEY  1<<PC3
#define START_KEY 1<<PC2
#define DOWN_KEY  1<<PC1
#define UP_KEY    1<<PC0

#define KEY_MASK (MODE_KEY | TIME_KEY | START_KEY | DOWN_KEY | UP_KEY)

/* detect long presses of start/pause key */
extern uint8_t holdTimer;
extern const uint8_t holdTimerThreshold;

uint8_t keyState;
uint8_t lastKeyState;
uint8_t keyPressed;
uint8_t keyReleased;


void init_keys(void);
void scan_keys(void);


#endif /* KEYS_H_ */
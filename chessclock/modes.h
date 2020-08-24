/*
 * modes.h
 *
 * Created: 24/08/2020 17:35:05
 *  Author: Joshua
 */ 


#ifndef MODES_H_
#define MODES_H_

#include <avr/io.h>

#include "settings.h"
#include "sound.h"

/* Editing modes go first to make retrieving strings simpler */
enum _state {EDIT_MODE, EDIT_DELAY, EDIT_BRIGHTNESS, EDIT_SOUND, EDIT_TIME, IDLE, GAME_ACTIVE, GAME_PAUSED, GAME_FINISHED} state;

typedef void (*switch_callback_t)();
typedef void (*tick_callback_t)();

extern switch_callback_t switch_callbacks[NUM_MODES];
extern tick_callback_t tick_callbacks[NUM_MODES];

extern void add_time(volatile gameTime baseTime, uint8_t incTime);

extern void on_switch_interrupt(uint8_t player, uint8_t otherPlayer, uint8_t playerLEDPin, uint8_t otherPlayerLEDPin);

extern void empty_switch_callback();
extern void increment_switch_callback();

extern void empty_tick_callback();
extern void simple_increment_tick_callback();

#endif /* MODES_H_ */
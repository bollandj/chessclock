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
	
extern volatile gameTime playerTime[2];
extern volatile uint8_t playerTicks[2];

extern volatile uint8_t currentPlayer;

extern volatile int8_t delay;        // countdown timer in simple delay/Bronstein modes
extern volatile uint8_t delayTicks;  // tick counter for delay
extern volatile uint8_t delayPassed; // has delay period elapsed?

extern volatile gameTime startTime;  // time at start of player's turn in Bronstein mode
extern volatile uint8_t startTicks;  // ticks at start of player's turn in Bronstein mode

typedef void (*callback_t)();

extern callback_t start_callbacks[NUM_MODES];
extern callback_t switch_callbacks[NUM_MODES];
extern callback_t tick_callbacks[NUM_MODES];

extern void add_time(volatile gameTime baseTime, uint8_t incTime);

extern void on_switch_interrupt(uint8_t player);
extern void on_tick(); // integral to most timing modes
extern void on_game_end(); // integral to most timing modes

/* Callback functions for any necessary initialisation before counting starts */
extern void empty_start_callback();
extern void delay_start_callback();
extern void bronstein_start_callback();
extern void hourglass_start_callback();
extern void countup_start_callback();

/* Callback functions called when switching players */
extern void empty_switch_callback();
extern void increment_switch_callback();
extern void delay_switch_callback();
extern void bronstein_switch_callback();
extern void hourglass_switch_callback();
extern void countup_switch_callback();

/* Callback functions called on each tick */
extern void empty_tick_callback();
extern void simple_tick_callback();
extern void delay_tick_callback();
extern void bronstein_tick_callback();
extern void hourglass_tick_callback();
extern void countup_tick_callback();

#endif /* MODES_H_ */
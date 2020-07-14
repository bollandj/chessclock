/*
 * settings.h
 *
 * Created: 07/07/2020 17:14:24
 *  Author: Joshua
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#define NUM_MODES 2
typedef enum _gameType {SIMPLE, INCREMENT, SIMPLE_DELAY, BRONSTEIN_DELAY} gameType;
extern const char *gameTypeNames[NUM_MODES];

// typedef struct
// {
// 	uint8_t ticks;
// 	int8_t  seconds;
// 	int8_t  tenSeconds;
// 	int8_t  minutes;
// 	int8_t  tenMinutes;
// 	int8_t  hours;
// 	int8_t  tenHours;
// } gameTime;

#define TEN_HOURS 0
#define HOURS 1
#define TEN_MINUTES 2
#define MINUTES 3
#define TEN_SECONDS 4
#define SECONDS 5

typedef int8_t gameTime[6];

volatile gameTime playerATime, playerBTime, delayTime, *currentPlayerTime;
volatile uint8_t playerATicks, playerBTicks, *currentPlayerTicks;

typedef struct
{
	gameTime initialTime; // time each player starts with 
	gameType gameMode;    // timing mode
	gameTime delay;	      // increment or delay time
} gameConfiguration;

gameConfiguration gameConfig; 

extern const gameConfiguration blitz3plus0Config;
extern const gameConfiguration blitz3plus2Config;
extern const gameConfiguration blitz5plus0Config;
extern const gameConfiguration blitz5plus3Config;

void store_config(); // 512 bytes of EEPROM space

void load_config();

#endif /* SETTINGS_H_ */
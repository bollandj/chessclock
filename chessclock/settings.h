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

#define NUM_SETTINGS 4
#define NUM_MODES 4

typedef enum _gameType {SIMPLE, INCREMENT, SIMPLE_DELAY, BRONSTEIN_DELAY} gameType;
	
extern const char settingsMenuNames[NUM_SETTINGS][5];
//extern char settingsMenuString[5];

extern const char gameTypeNames[NUM_MODES][5];
//extern char gameTypeString[5];

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

typedef struct
{
	uint8_t displayBrightness; // display PWM value
	uint8_t soundOn;           // make sound?
} deviceConfiguration;

deviceConfiguration deviceConfig;

extern const gameConfiguration blitz3plus0Config;
extern const gameConfiguration blitz3plus2Config;
extern const gameConfiguration blitz5plus0Config;
extern const gameConfiguration blitz5plus3Config;

void store_config(); // 512 bytes of EEPROM space

void load_config();

#endif /* SETTINGS_H_ */
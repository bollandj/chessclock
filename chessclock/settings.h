/*
 * settings.h
 *
 * Created: 07/07/2020 17:14:24
 *  Author: Joshua
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

typedef enum _gameType {SIMPLE, INCREMENT, SIMPLE_DELAY, BRONSTEIN_DELAY} gameType;

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

const gameConfiguration blitz3plus0Config =
{
	{0, 0, 0, 3, 0, 0},
	SIMPLE,
	{0, 0, 0, 0, 0, 0}
};

const gameConfiguration blitz3plus2Config =
{
	{0, 0, 0, 3, 0, 0},
	INCREMENT,
	{0, 0, 0, 0, 0, 2}
};

const gameConfiguration blitz5plus0Config =
{
	{0, 0, 0, 5, 0, 0},
	SIMPLE,
	{0, 0, 0, 0, 0, 0}
};

const gameConfiguration blitz5plus3Config =
{
	{0, 0, 0, 5, 0, 0},
	INCREMENT,
	{0, 0, 0, 0, 0, 3}
};


#endif /* SETTINGS_H_ */
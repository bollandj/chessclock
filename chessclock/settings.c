/*
 * settings.c
 *
 * Created: 07/07/2020 17:17:06
 *  Author: Joshua
 */ 

#include "settings.h"

const char *gameTypeNames[NUM_MODES] = {"SMPL", "INCR"};

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


void store_config()
{
	cli();	
	eeprom_update_block((const void *)&gameConfig, (void *)0, sizeof(gameConfiguration));	
	sei();	
}

void load_config()
{
	cli();	
	eeprom_read_block((void *)&gameConfig, (const void *)0, sizeof(gameConfiguration));	
	sei();
}

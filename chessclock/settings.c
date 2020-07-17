/*
 * settings.c
 *
 * Created: 07/07/2020 17:17:06
 *  Author: Joshua
 */ 

#include "settings.h"

const char settingsMenuNames[NUM_SETTINGS][5] = {"MODE", "DEL ", "BRT ", "SND "};
//char settingsMenuString[5];

const char gameTypeNames[NUM_MODES][5] = {"SMPL", "INCR", "DEL ", "BRON"};
//char gameTypeString[5];

const char offOnStrings[2][5] = {" OFF", "  ON"};

const gameConfiguration blitz3plus0Config =
{
	{0, 0, 0, 3, 0, 0},
	SIMPLE,
	0
};

const gameConfiguration blitz3plus2Config =
{
	{0, 0, 0, 3, 0, 0},
	INCREMENT,
	2
};

const gameConfiguration blitz5plus0Config =
{
	{0, 0, 0, 5, 0, 0},
	SIMPLE,
	0
};

const gameConfiguration blitz5plus3Config =
{
	{0, 0, 0, 5, 0, 0},
	INCREMENT,
	3
};


void store_config()
{
	cli();	
	eeprom_update_block((const void *)&gameConfig, (void *)0, sizeof(gameConfiguration));
	eeprom_update_block((const void *)&deviceConfig, (void *)256, sizeof(deviceConfiguration));	
	sei();	
}

void load_config()
{
	cli();	
	eeprom_read_block((void *)&gameConfig, (const void *)0, sizeof(gameConfiguration));	
	eeprom_read_block((void *)&deviceConfig, (const void *)256, sizeof(deviceConfiguration));
	sei();
}

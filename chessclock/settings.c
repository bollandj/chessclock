/*
 * settings.c
 *
 * Created: 07/07/2020 17:17:06
 *  Author: Joshua
 */ 

#include "settings.h"

const char settingsMenuNames[NUM_SETTINGS][5] = {"MODE", "DEL ", "BRT ", "SND "};

const char gameTypeNames[NUM_MODES][5] = {"SMPL", "INCR", "DEL ", "BRON", "HRGL", "CTUP"};

const char offOnStrings[2][5] = {" OFF", "  ON"};
	
uint8_t timeEditCursor=0;



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

/* Run first time only to initialise EEPROM */
void init_config()
{
	gameConfig = blitz3plus2Config;
	deviceConfiguration dc = {5, 1};
	deviceConfig = dc;
	
	cli();
	eeprom_update_block((const void *)&gameConfig, (void *)0, sizeof(gameConfiguration));
	eeprom_update_block((const void *)&deviceConfig, (void *)256, sizeof(deviceConfiguration));
	sei();
}

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

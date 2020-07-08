/*
 * chessclock.c
 *
 * Created: 03/07/2020 20:23:24
 * Author : Joshua
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "display.h"
#include "keys.h"
#include "settings.h"

uint8_t blink=0x00;
uint8_t blinkTimer=0;
const uint8_t blinkThreshold=30;

enum _state {IDLE, EDIT_TIME, EDIT_SETTINGS, GAME_ACTIVE, GAME_PAUSED, GAME_FINISHED} state;

typedef struct 
{
	uint16_t moves;
	uint8_t isWhite;
} playerData;

volatile playerData playerAData, playerBData, *currentPlayerData;

void init_timer(void)
{
	ASSR = 1<<AS2; // enable asynchronous mode
	
	TCCR2B |= 1<<CS20;         // /1 prescaler
	while (ASSR & 1<<TCR2BUB); // wait for register update

	TIFR2  = 1<<TOV2;   // clear interrupt flag	
	//TIMSK2 = 1<<TOIE2;  // enable Timer2 overflow interrupt
}

/* increments baseTime by incTime (ignoring ticks) */
void add_time(volatile gameTime *baseTime, gameTime incTime)
{
	(*baseTime)[SECONDS] += incTime[SECONDS];
	if ((*baseTime)[SECONDS] > 9) 
	{
		(*baseTime)[SECONDS] = 0;
		(*baseTime)[TEN_SECONDS]++;
	}
	
	(*baseTime)[TEN_SECONDS] += incTime[TEN_SECONDS];
	if ((*baseTime)[TEN_SECONDS] > 5)
	{
		(*baseTime)[TEN_SECONDS] = 0;
		(*baseTime)[MINUTES]++;
	}
	
	(*baseTime)[MINUTES] += incTime[MINUTES];
	if ((*baseTime)[MINUTES] > 9)
	{
		(*baseTime)[MINUTES] = 0;
		(*baseTime)[TEN_MINUTES]++;
	}
	
	(*baseTime)[TEN_MINUTES] += incTime[TEN_MINUTES];
	if ((*baseTime)[TEN_MINUTES] > 5)
	{
		(*baseTime)[TEN_MINUTES] = 0;
		(*baseTime)[HOURS]++;
	}
	
	(*baseTime)[HOURS] += incTime[HOURS];
	if ((*baseTime)[HOURS] > 9)
	{
		(*baseTime)[HOURS] = 0;
		(*baseTime)[TEN_HOURS]++;
	}
	
	(*baseTime)[TEN_HOURS] += incTime[TEN_HOURS];
	if ((*baseTime)[TEN_HOURS] > 9)
	{
		(*baseTime)[TEN_HOURS]   = 0;
		(*baseTime)[HOURS]       = 0;
		(*baseTime)[TEN_MINUTES] = 0;
		(*baseTime)[MINUTES]     = 0;
		(*baseTime)[TEN_SECONDS] = 0;
		(*baseTime)[SECONDS]     = 0;
	}
}

/* returns */
//int8_t cmp_time()

void reset(void)
{
	/* reset time */
	for (uint8_t i = 0; i < 6; i++)
	{
		uint8_t initialTimeComponent = gameConfig.initialTime[i];
		playerATime[i] = initialTimeComponent;
		playerBTime[i] = initialTimeComponent;
	}

	/* reset ticks */
	playerATicks = 0;
	playerBTicks = 0;
	
	/* reset move counts */
	playerAData.moves = 0;
	playerBData.moves = 0;
}

void write_time(void)
{	
	for (uint8_t i = 0; i < 4; i++)
	{
		displayBuffer[i] = playerATime[i+2];
		displayBuffer[i+4] = playerBTime[i+2];
	}
}


int main(void)
{		
	/* default to player A as white/starting */
	
	currentPlayerTicks = &playerATicks;
	currentPlayerTime = &playerATime;
	currentPlayerData = &playerAData;
	
	DDRB |= 1<<PB0 | 1<<PB1;
	PORTB &= ~(1<<PB1);
	PORTB |= 1<<PB0;
	
	gameConfig = blitz5plus3Config;
	
	reset();
	
	init_display();
	init_timer();
	init_keys();
	
	sei();
	
    while (1) 
    {
		scan_keys();
		
		switch (state)
		{
			case IDLE:
			if (keyPressed & START_KEY)
			{
				TIMSK2 = 1<<TOIE2;
				state = GAME_ACTIVE;
			}
			else if (keyPressed & TIME_KEY)
			{
				state = EDIT_TIME;
			}
			
			write_time();
			break;
			
			case EDIT_TIME:
			if (keyPressed & START_KEY)
			{
				state = IDLE;	
			}
			break;
			
			case EDIT_SETTINGS:
			if (keyPressed & START_KEY)
			{
				state = IDLE;
			}
			break;
			
			case GAME_ACTIVE:
			if (keyPressed & START_KEY)
			{
				TIMSK2 = 0x00;
				state = GAME_PAUSED;	
			}
			
			write_time();
			break;
			
			case GAME_PAUSED:
			if (keyPressed & START_KEY)
			{
				TIMSK2 = 1<<TOIE2;
				state = GAME_ACTIVE;
			}
			
			write_time();
			break;
			
			case GAME_FINISHED:
			if (keyPressed & START_KEY)
			{
				reset();
				state = IDLE;
			}
			
			write_time();
			break;
		}
		
		
		
		blinkTimer++;
		if (blinkTimer > blinkThreshold)
		{
			blinkTimer = 0;
			blink ^= 0x0F;
		}		
		_delay_ms(15);	
    }
}

/* Player A's button */
ISR(INT0_vect) 
{	
	switch (state)
	{
		case GAME_ACTIVE:
		if (currentPlayerTime == &playerATime)
		{
			switch (gameConfig.gameMode)
			{
				case SIMPLE:
				
				break;
				
				case INCREMENT:
					add_time(currentPlayerTime, gameConfig.delay);
				break;
				
				case SIMPLE_DELAY:
				
				break;
				
				case BRONSTEIN_DELAY:
				
				break;
			}				
		}
		break;
		
		default:			
		break;
	}
	
	currentPlayerTime  = &playerBTime; // start decrementing other player's time instead
	currentPlayerTicks = &playerBTicks;
		
	PORTB |= 1<<PB0;
	PORTB &= ~(1<<PB1);	
}

/* Player B's button */
ISR(INT1_vect)
{
	switch (state)
	{		
		case GAME_ACTIVE:
		if (currentPlayerTime == &playerBTime)
		{			
			switch (gameConfig.gameMode)
			{
				case SIMPLE:
				
				break;
			
				case INCREMENT:
					add_time(currentPlayerTime, gameConfig.delay);
				break;
			
				case SIMPLE_DELAY:
			
				break;
			
				case BRONSTEIN_DELAY:
				
				break;
			}			
		}
		break;
		
		default:
		break;
	}
	
	currentPlayerTime  = &playerATime; // start decrementing other player's time instead	
	currentPlayerTicks = &playerATicks;
		
	PORTB |= 1<<PB1;
	PORTB &= ~(1<<PB0);
}

/* TODO: implement simple/Bronstein delay by decrementing delay time in addition to/as well as current player time */
ISR(TIMER2_OVF_vect)
{	
	if (++(*currentPlayerTicks) > 127)
	{
		*currentPlayerTicks = 0;
		
		if (--(*currentPlayerTime)[SECONDS] < 0)
		{
			(*currentPlayerTime)[SECONDS] = 9;
		
			if (--(*currentPlayerTime)[TEN_SECONDS] < 0)
			{
				(*currentPlayerTime)[TEN_SECONDS] = 5;
			
				if (--(*currentPlayerTime)[MINUTES] < 0)
				{
					(*currentPlayerTime)[MINUTES] = 9;
				
					if (--(*currentPlayerTime)[TEN_MINUTES] < 0)
					{					
						(*currentPlayerTime)[TEN_MINUTES] = 5;
					
						if (--(*currentPlayerTime)[HOURS] < 0)
						{
							(*currentPlayerTime)[HOURS] = 9;
						
							if(--(*currentPlayerTime)[TEN_HOURS] < 0)
							{
								(*currentPlayerTime)[TEN_HOURS]   = 0;
								(*currentPlayerTime)[HOURS]       = 0;
								(*currentPlayerTime)[TEN_MINUTES] = 0;
								(*currentPlayerTime)[MINUTES]     = 0;
								(*currentPlayerTime)[TEN_SECONDS] = 0;
								(*currentPlayerTime)[SECONDS]     = 0;	
								
								TIMSK2 = 0x00; // disable further ticks
								state = GAME_FINISHED;
							}			
						}				
					}
				}
			}
		}
	}		
	
}


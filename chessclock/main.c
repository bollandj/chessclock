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
#include "sound.h"

uint8_t timeEditCursor=0;

/* Editing modes go first to make retrieving strings simpler */
enum _state {EDIT_MODE, EDIT_DELAY, EDIT_BRIGHTNESS, EDIT_SOUND, EDIT_TIME, IDLE, GAME_ACTIVE, GAME_PAUSED, GAME_FINISHED} state;

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
		(*baseTime)[SECONDS] -= 10;
		(*baseTime)[TEN_SECONDS]++;
	}
	
	(*baseTime)[TEN_SECONDS] += incTime[TEN_SECONDS];
	if ((*baseTime)[TEN_SECONDS] > 5)
	{
		(*baseTime)[TEN_SECONDS] -= 6;
		(*baseTime)[MINUTES]++;
	}
	
	(*baseTime)[MINUTES] += incTime[MINUTES];
	if ((*baseTime)[MINUTES] > 9)
	{
		(*baseTime)[MINUTES] -= 10;
		(*baseTime)[TEN_MINUTES]++;
	}
	
	(*baseTime)[TEN_MINUTES] += incTime[TEN_MINUTES];
	if ((*baseTime)[TEN_MINUTES] > 5)
	{
		(*baseTime)[TEN_MINUTES] -= 6;
		(*baseTime)[HOURS]++;
	}
	
	(*baseTime)[HOURS] += incTime[HOURS];
	if ((*baseTime)[HOURS] > 9)
	{
		(*baseTime)[HOURS] -= 10;
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
		playerATime[i] = gameConfig.initialTime[i];
		playerBTime[i] = gameConfig.initialTime[i];
	}

	/* reset ticks */
	playerATicks = 0;
	playerBTicks = 0;
	
	/* reset move counts */
	playerAData.moves = 0;
	playerBData.moves = 0;
}

int main(void)
{		
	/* default to player A as white/starting */
	
	currentPlayerTicks = &playerATicks;
	currentPlayerTime = &playerATime;
	currentPlayerData = &playerAData;
	
	state = IDLE;
	
	DDRD |= 1<<PD6 | 1<<PD7;
	PORTD &= ~(1<<PD7);
	PORTD |= 1<<PD6;
	
	load_config();
	
	reset();
	
	init_display();
	init_keys();
	init_timer();
	init_sound();
	
	sei();
	
    while (1) 
    {
		_delay_ms(15);
		
		scan_keys();
		do_blink();
		
		switch (state)
		{
			/* Idle */
			/* Ready to start a game */
			case IDLE:
			if (keyPressed & START_KEY)
			{
				TIMSK2 = 1<<TOIE2;
				
				state = GAME_ACTIVE;
			}
			else if (keyPressed & MODE_KEY)
			{				
				state = EDIT_MODE;
			}
			else if (keyPressed & TIME_KEY)
			{
				timeEditCursor = 0;
				blinkMask[0] = 0xFF;
				blinkMask[4] = 0xFF;
								
				state = EDIT_TIME;
			}			
			write_time(0);
			break;
			
			/* Time edit mode */
			/* Edit initial time for one or both players */
			case EDIT_TIME:
			if (keyPressed & START_KEY)
			{
				/* stop blinking */
				blinkMask[timeEditCursor] = 0x00;
				blinkMask[timeEditCursor+4] = 0x00;
				
				/* save settings */
				store_config();
				
				state = IDLE;	
			}
			else if (keyPressed & TIME_KEY)
			{
				blinkMask[timeEditCursor] = 0x00;
				blinkMask[timeEditCursor+4] = 0x00;
				
				timeEditCursor++;
				timeEditCursor &= 0x03;
				
				blinkMask[timeEditCursor] = 0xFF;
				blinkMask[timeEditCursor+4] = 0xFF;
			}
			else if (keyPressed & (UP_KEY | DOWN_KEY))
			{
				int8_t timeComponent;
				
				uint8_t limit = 5;                    // 9 for units, 5 for seconds
				if (timeEditCursor & 0x01) limit = 9; // even = units, odd = tens
				
				if (keyPressed & UP_KEY)
				{
					timeComponent = ++playerATime[timeEditCursor+2];
					if (timeComponent > limit) timeComponent = 0;
				}
				else
				{
					timeComponent = --playerATime[timeEditCursor+2];
					if (timeComponent < 0) timeComponent = limit;
				}
				
				gameConfig.initialTime[timeEditCursor+2] = timeComponent;
				
				playerATime[timeEditCursor+2] = timeComponent;
				playerBTime[timeEditCursor+2] = timeComponent;
			}
			
			write_time(1);
			break;
			
			/* Edit game mode */
			/* Select between different clock modes */
			case EDIT_MODE:			
			if (keyPressed & START_KEY)
			{
				/* save settings */
				store_config();
				
				state = IDLE;
			}
			else if (keyPressed & MODE_KEY)
			{			
				state++;
			}					
			else if (keyPressed & (UP_KEY | DOWN_KEY))
			{	
				if (keyPressed & UP_KEY)
				{
					gameConfig.gameMode++;
					if (gameConfig.gameMode > NUM_MODES) gameConfig.gameMode = 0;
				}
				else
				{
					gameConfig.gameMode--;
					if (gameConfig.gameMode < 0) gameConfig.gameMode = NUM_MODES;
				}
			}		
			
			write_string(settingsMenuNames[state], 0, 4);		
			write_string(gameTypeNames[gameConfig.gameMode], 4, 8);
			break;
			
			/* Edit time increment/delay */
			/* Change increment or delay (simple/Bronstein modes) time */
			case EDIT_DELAY:
			if (keyPressed & START_KEY)
			{
				/* save settings */
				store_config();
				
				state = IDLE;
			}
			else if (keyPressed & MODE_KEY)
			{		
				state++;
			}
			
			write_string(settingsMenuNames[state], 0, 4);
			break;
			
			/* Edit display brightness */
			/* Change PWM value */
			case EDIT_BRIGHTNESS:
			if (keyPressed & START_KEY)
			{
				/* save settings */
				store_config();
				
				state = IDLE;
			}
			else if (keyPressed & MODE_KEY)
			{				
				state++;
			}
			
			write_string(settingsMenuNames[state], 0, 4);
			break;
			
			/* Edit sound */
			/* Turn sound on or off */
			case EDIT_SOUND:
			if (keyPressed & START_KEY)
			{
				/* save settings */
				store_config();
				
				state = IDLE;
			}
			else if (keyPressed & MODE_KEY)
			{				
				state = EDIT_MODE; // wrap around after last setting in list
			}
			
			write_string(settingsMenuNames[state], 0, 4);
			break;
			
			/* Game active */
			/* Clock is currently counting down */
			case GAME_ACTIVE:
			if (keyPressed & START_KEY)
			{
				TIMSK2 = 0x00;
				state = GAME_PAUSED;	
			}
			
			write_time(0);
			break;
			
			/* Game paused */
			/* Clock countdown for both players is paused */
			case GAME_PAUSED:
			if (holdTimer >= holdTimerThreshold)
			{				
				reset();
				state = IDLE;	
			}
			else if (keyPressed & START_KEY)
			{
				TIMSK2 = 1<<TOIE2;
				state = GAME_ACTIVE;
			}
			
			write_time(0);
			break;
			
			/* Game finished */
			/* One player's time has run out */
			case GAME_FINISHED:
			if (keyPressed & START_KEY)
			{
				reset();
				state = IDLE;
			}
			
			write_time(0);
			break;
		}				
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
		
	PORTD |= 1<<PD7;
	PORTD &= ~(1<<PD6);	
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
		
	PORTD |= 1<<PD6;
	PORTD &= ~(1<<PD7);
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


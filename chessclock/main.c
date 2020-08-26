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
#include "modes.h"
#include "settings.h"
#include "sound.h"

void init_timer(void)
{
	ASSR = 1<<AS2; // enable asynchronous mode
	
	TCCR2B |= 1<<CS20;         // /1 prescaler
	while (ASSR & 1<<TCR2BUB); // wait for register update

	TIFR2  = 1<<TOV2;   // clear interrupt flag	
	//TIMSK2 = 1<<TOIE2;  // enable Timer2 overflow interrupt
}

void reset(void)
{	
	/* reset time */
	for (uint8_t i = 0; i < 6; i++)
	{
		playerTime[PLAYER_A][i] = gameConfig.initialTime[i];
		playerTime[PLAYER_B][i] = gameConfig.initialTime[i];
	}

	/* reset ticks */
	playerTicks[PLAYER_A] = 127;
	playerTicks[PLAYER_B] = 127;
}

int main(void)
{			
	/* default to player A as white/starting */
	
	currentPlayer = PLAYER_A;
	
	state = IDLE;
	
	DDRD |= 1<<PD6 | 1<<PD7;
	PORTD &= ~(1<<PD7);
	PORTD |= 1<<PD6;
	
	load_config();
	
	reset();
	
	init_display(deviceConfig.brightness);
	init_keys();
	init_timer();
	init_sound();
	
	sei();
	
    while (1) 
    {					
		scan_keys();		
		
		if ((keyPressed & KEY_MASK) && deviceConfig.soundOn) beep(2);
		
		switch (state)
		{
			/* Idle */
			/* Ready to start a game */
			case IDLE:
			if (keyPressed & START_KEY)
			{
				start_callbacks[gameConfig.gameMode](); // gamemode-specific initialisation
				/* Start counting */
				TIMSK2 = 1<<TOIE2;			
				state = GAME_ACTIVE;
				break;
			}
			else if (keyPressed & MODE_KEY)
			{								
				state = EDIT_MODE;
				break;
			}
			else if (keyPressed & TIME_KEY)
			{
				timeEditCursor = 0;
				blinkMask[0] = 0xFF;
				blinkMask[4] = 0xFF;
								
				state = EDIT_TIME;
				break;
			}
				
			COLON_ON();		
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
				break;	
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
					timeComponent = ++playerTime[PLAYER_A][timeEditCursor+2];
					if (timeComponent > limit) timeComponent = 0;
				}
				else
				{
					timeComponent = --playerTime[PLAYER_A][timeEditCursor+2];
					if (timeComponent < 0) timeComponent = limit;
				}
				
				gameConfig.initialTime[timeEditCursor+2] = timeComponent;
				
				playerTime[PLAYER_A][timeEditCursor+2] = timeComponent;
				playerTime[PLAYER_B][timeEditCursor+2] = timeComponent;
			}
			
			COLON_ON();	
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
				break;
			}
			else if (keyPressed & MODE_KEY)
			{			
				state++;
				break;
			}						
			else if (keyPressed & UP_KEY)
			{			
				if (gameConfig.gameMode == NUM_MODES-1) gameConfig.gameMode = 0;
				else gameConfig.gameMode++;
			}
			else if (keyPressed & DOWN_KEY)
			{
				
				if (gameConfig.gameMode == 0) gameConfig.gameMode = NUM_MODES-1;
				else gameConfig.gameMode--;
			}
				
			COLON_OFF();		
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
				break;
			}
			else if (keyPressed & MODE_KEY)
			{		
				state++;
				break;
			}
			else if (keyPressed & UP_KEY)
			{
				if      (gameConfig.delay < 20)  gameConfig.delay++;
				else if (gameConfig.delay < 45)  gameConfig.delay += 5;
				else if (gameConfig.delay < 60)  gameConfig.delay += 15; 
				else if (gameConfig.delay < 120) gameConfig.delay += 30; 	
			}
			else if (keyPressed & DOWN_KEY)
			{
				if      (gameConfig.delay > 60) gameConfig.delay -= 30;
				else if (gameConfig.delay > 45) gameConfig.delay -= 15;
				else if (gameConfig.delay > 20) gameConfig.delay -= 5;
				else if (gameConfig.delay > 0)  gameConfig.delay--; 	
			}
				
			COLON_OFF();		
			write_string(settingsMenuNames[state], 0, 4);
			write_number_8(gameConfig.delay, 4);
			break;
			
			/* Edit display brightness */
			/* Change PWM value */
			case EDIT_BRIGHTNESS:
			if (keyPressed & START_KEY)
			{
				/* save settings */
				store_config();
				
				state = IDLE;
				break;
			}
			else if (keyPressed & MODE_KEY)
			{				
				state++;
				break;
			}
			else if (keyPressed & UP_KEY)
			{
				if (deviceConfig.brightness < 10) deviceConfig.brightness++;
				OCR0B = deviceConfig.brightness << 2;	
			}
			else if (keyPressed & DOWN_KEY)
			{
				if (deviceConfig.brightness > 1) deviceConfig.brightness--;
				OCR0B = deviceConfig.brightness << 2;	
			}
			
			COLON_OFF();
			write_string(settingsMenuNames[state], 0, 4);
			write_number_8(deviceConfig.brightness, 4);
			break;
			
			/* Edit sound */
			/* Turn sound on or off */
			case EDIT_SOUND:
			if (keyPressed & START_KEY)
			{
				/* save settings */
				store_config();
				
				state = IDLE;
				break;
			}
			else if (keyPressed & MODE_KEY)
			{				
				state = EDIT_MODE; // wrap around after last setting in list
				break;
			}
			else if (keyPressed & (UP_KEY | DOWN_KEY))
			{
				deviceConfig.soundOn++;	
				deviceConfig.soundOn &= 0x01;
				beep(2); // extra beep required here as sound was disabled at top of loop	
			}
			
			COLON_OFF();
			write_string(settingsMenuNames[state], 0, 4);
			write_string(offOnStrings[deviceConfig.soundOn], 4, 8);
			break;
			
			/* Game active */
			/* Clock is currently counting down */
			case GAME_ACTIVE:
			if (keyPressed & START_KEY)
			{
				TIMSK2 = 0x00;
				state = GAME_PAUSED;
				break;	
			}
			
			COLON_ON();
			write_time(0);
			break;
			
			/* Game paused */
			/* Clock countdown for both players is paused */
			case GAME_PAUSED:
			if (holdTimer >= holdTimerThreshold)
			{				
				reset();
				beep(12);
				state = IDLE;
				break;	
			}
			else if (keyPressed & START_KEY)
			{
				TIMSK2 = 1<<TOIE2;
				state = GAME_ACTIVE;
				break;
			}
			
			COLON_ON();
			write_time(0);
			break;
			
			/* Game finished */
			/* One player's time has run out */
			case GAME_FINISHED:
			if (keyPressed & START_KEY)
			{
				reset();
				beep(12);
				state = IDLE;
				break;
			}
			
			COLON_ON();
			write_time(0);
			break;
		}
							
		update_blink();
		update_beep();
		
		_delay_ms(15);				
    }
}

/* Player A's button */
ISR(INT0_vect) 
{	
	on_switch_interrupt(PLAYER_A, PLAYER_B, PD6, PD7);	
}

/* Player B's button */
ISR(INT1_vect)
{
	on_switch_interrupt(PLAYER_B, PLAYER_A, PD7, PD6);
}

ISR(TIMER2_OVF_vect)
{	
	tick_callbacks[gameConfig.gameMode]();				
}


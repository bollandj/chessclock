/*
 * modes.c
 *
 * Created: 24/08/2020 17:34:57
 *  Author: Joshua
 */ 

#include "modes.h"

volatile gameTime playerTime[2];
volatile uint8_t playerTicks[2];

volatile uint8_t currentPlayer;

volatile int8_t delay;

callback_t start_callbacks[NUM_MODES] = {empty_start_callback, empty_start_callback, delay_start_callback, bronstein_start_callback};
callback_t switch_callbacks[NUM_MODES] = {empty_switch_callback, increment_switch_callback, empty_switch_callback, empty_switch_callback};
callback_t tick_callbacks[NUM_MODES] = {simple_tick_callback, simple_tick_callback, delay_tick_callback, bronstein_tick_callback};
	
/* increments baseTime by incTime (ignoring ticks) */
void add_time(volatile gameTime baseTime, uint8_t incTime)
{
	baseTime[SECONDS] += incTime;
		
	while (baseTime[SECONDS] > 9)
	{
		baseTime[SECONDS] -= 10;
		baseTime[TEN_SECONDS]++;
	}
		
	while (baseTime[TEN_SECONDS] > 5)
	{
		baseTime[TEN_SECONDS] -= 6;
		baseTime[MINUTES]++;
	}
		
	while (baseTime[MINUTES] > 9)
	{
		baseTime[MINUTES] -= 10;
		baseTime[TEN_MINUTES]++;
	}
		
	while (baseTime[TEN_MINUTES] > 5)
	{
		baseTime[TEN_MINUTES] -= 6;
		baseTime[HOURS]++;
	}
		
	while (baseTime[HOURS] > 9)
	{
		baseTime[HOURS] -= 10;
		baseTime[TEN_HOURS]++;
	}
		
	if (baseTime[TEN_HOURS] > 9)
	{
		baseTime[TEN_HOURS]   = 9;
		baseTime[HOURS]       = 9;
		baseTime[TEN_MINUTES] = 9;
		baseTime[MINUTES]     = 9;
		baseTime[TEN_SECONDS] = 9;
		baseTime[SECONDS]     = 9;
	}
}

void on_switch_interrupt(uint8_t player, uint8_t otherPlayer, uint8_t playerLEDPin, uint8_t otherPlayerLEDPin)
{
	switch (state)
	{
		case GAME_ACTIVE:
		if (player == currentPlayer) switch_callbacks[gameConfig.gameMode](); // only called on player change
		// fallthrough
		default:
		currentPlayer = otherPlayer;
		
		PORTD |= 1<<otherPlayerLEDPin;
		PORTD &= ~(1<<playerLEDPin);
		break;
	}	
}

void on_game_end()
{
	TIMSK2 = 0x00; // disable further ticks
	state = GAME_FINISHED;
	beep(24);		
}

/* Placeholder callback for simple and increment modes */
void empty_start_callback()
{
	
}


/* callback */
void delay_start_callback()
{
	delay = gameConfig.delay; // reset delay counter		
}

/* callback */	
void bronstein_start_callback()
{
	
}

/* Placeholder callback */
void empty_switch_callback()
{
	
}

/* Callback for increment mode */	
void increment_switch_callback()
{
	add_time(playerTime[currentPlayer], gameConfig.delay);		
}

/* callback */
void delay_switch_callback()
{
	delay = gameConfig.delay; // reset delay counter	
}

/* callback */
void bronstein_switch_callback()
{
	
}

/* Placeholder callback */
void empty_tick_callback()
{
	
}

/* Callback for simple and increment modes */
void simple_tick_callback()
{
	if (++playerTicks[currentPlayer] > 127)
	{
		playerTicks[currentPlayer] = 0;
		
		if (--playerTime[currentPlayer][SECONDS] < 0)
		{
			playerTime[currentPlayer][SECONDS] = 9;
			
			if (--playerTime[currentPlayer][TEN_SECONDS] < 0)
			{
				playerTime[currentPlayer][TEN_SECONDS] = 5;
				
				if (--playerTime[currentPlayer][MINUTES] < 0)
				{
					playerTime[currentPlayer][MINUTES] = 9;
					
					if (--playerTime[currentPlayer][TEN_MINUTES] < 0)
					{
						playerTime[currentPlayer][TEN_MINUTES] = 5;
						
						if (--playerTime[currentPlayer][HOURS] < 0)
						{
							playerTime[currentPlayer][HOURS] = 9;
							
							if(--playerTime[currentPlayer][TEN_HOURS] < 0)
							{
								playerTime[currentPlayer][TEN_HOURS]   = 0;
								playerTime[currentPlayer][HOURS]       = 0;
								playerTime[currentPlayer][TEN_MINUTES] = 0;
								playerTime[currentPlayer][MINUTES]     = 0;
								playerTime[currentPlayer][TEN_SECONDS] = 0;
								playerTime[currentPlayer][SECONDS]     = 0;
														
								on_game_end();
							}
						}
					}
				}
			}
		}
	}
}

/* callback */
void delay_tick_callback()
{
	if (++playerTicks[currentPlayer] > 127)
	{
		playerTicks[currentPlayer] = 0;
		
		if (delay < 0)
		{
			if (--playerTime[currentPlayer][SECONDS] < 0)
			{
				playerTime[currentPlayer][SECONDS] = 9;
				
				if (--playerTime[currentPlayer][TEN_SECONDS] < 0)
				{
					playerTime[currentPlayer][TEN_SECONDS] = 5;
					
					if (--playerTime[currentPlayer][MINUTES] < 0)
					{
						playerTime[currentPlayer][MINUTES] = 9;
						
						if (--playerTime[currentPlayer][TEN_MINUTES] < 0)
						{
							playerTime[currentPlayer][TEN_MINUTES] = 5;
							
							if (--playerTime[currentPlayer][HOURS] < 0)
							{
								playerTime[currentPlayer][HOURS] = 9;
								
								if(--playerTime[currentPlayer][TEN_HOURS] < 0)
								{
									playerTime[currentPlayer][TEN_HOURS]   = 0;
									playerTime[currentPlayer][HOURS]       = 0;
									playerTime[currentPlayer][TEN_MINUTES] = 0;
									playerTime[currentPlayer][MINUTES]     = 0;
									playerTime[currentPlayer][TEN_SECONDS] = 0;
									playerTime[currentPlayer][SECONDS]     = 0;
									
									on_game_end();
								}
							}
						}
					}
				}
			}
		}
		else
		{
			delay--;
		}
	}	
}

/* callback */
void bronstein_tick_callback()
{
	if (++playerTicks[currentPlayer] > 127)
	{
		playerTicks[currentPlayer] = 0;
		
		if (--playerTime[currentPlayer][SECONDS] < 0)
		{
			playerTime[currentPlayer][SECONDS] = 9;
			
			if (--playerTime[currentPlayer][TEN_SECONDS] < 0)
			{
				playerTime[currentPlayer][TEN_SECONDS] = 5;
				
				if (--playerTime[currentPlayer][MINUTES] < 0)
				{
					playerTime[currentPlayer][MINUTES] = 9;
					
					if (--playerTime[currentPlayer][TEN_MINUTES] < 0)
					{
						playerTime[currentPlayer][TEN_MINUTES] = 5;
						
						if (--playerTime[currentPlayer][HOURS] < 0)
						{
							playerTime[currentPlayer][HOURS] = 9;
							
							if(--playerTime[currentPlayer][TEN_HOURS] < 0)
							{
								playerTime[currentPlayer][TEN_HOURS]   = 0;
								playerTime[currentPlayer][HOURS]       = 0;
								playerTime[currentPlayer][TEN_MINUTES] = 0;
								playerTime[currentPlayer][MINUTES]     = 0;
								playerTime[currentPlayer][TEN_SECONDS] = 0;
								playerTime[currentPlayer][SECONDS]     = 0;
								
								on_game_end();
							}
						}
					}
				}
			}
		}
	}
	
}
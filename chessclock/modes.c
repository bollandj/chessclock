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
volatile uint8_t delayTicks;
volatile uint8_t delayPassed;

volatile gameTime startTime;
volatile uint8_t startTicks;

callback_t start_callbacks[NUM_MODES] = 
{
	empty_start_callback, 
	empty_start_callback, 
	delay_start_callback, 
	bronstein_start_callback, 
	hourglass_start_callback, 
	countup_start_callback
};
callback_t switch_callbacks[NUM_MODES] = 
{
	empty_switch_callback, 
	increment_switch_callback, 
	delay_switch_callback,
	bronstein_switch_callback, 
	hourglass_switch_callback, 
	countup_switch_callback
};
callback_t tick_callbacks[NUM_MODES] = 
{
	simple_tick_callback, 
	simple_tick_callback, 
	delay_tick_callback, 
	bronstein_tick_callback, 
	hourglass_tick_callback, 
	countup_tick_callback
};
	
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

void on_switch_interrupt(uint8_t player)
{
	if (player == currentPlayer) // player change
	{
		if (state == GAME_ACTIVE) switch_callbacks[gameConfig.gameMode](); // only called on player change during active game
		
		currentPlayer = player ^ 0x01; // switch players
		
		PORTD |= 1<<(PD6+player);
		PORTD &= ~(1<<(PD7-player));	
	}
}

void on_tick()
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

void on_game_end()
{
	TIMSK2 = 0x00; // disable further ticks
	state = GAME_FINISHED;
	beep(24);		
}

/* Placeholder callback for simple and increment modes (no additional behaviour required on game start) */
void empty_start_callback()
{
	
}

/* Simple delay start callback */
void delay_start_callback()
{
	delay = gameConfig.delay; // reset delay counter
	delayTicks = 127;
	delayPassed = 0;		
}

/* Bronstein delay start callback */	
void bronstein_start_callback()
{
	for (uint8_t i = 0; i < 6; i++) startTime[i] = gameConfig.initialTime[i];
	startTicks = 127;
	
	delay = gameConfig.delay; 
	delayTicks = 127;
	delayPassed = 0;	
}

/* Hourglass start callback */
void hourglass_start_callback()
{
	
}

/* Count up start callback */
void countup_start_callback()
{

}

/* Placeholder switch callback */
void empty_switch_callback()
{
	
}

/* Increment switch callback */	
void increment_switch_callback()
{
	add_time(playerTime[currentPlayer], gameConfig.delay);		
}

/* Simple delay switch callback */
void delay_switch_callback()
{
	delay = gameConfig.delay; 
	delayTicks = 127;	
	delayPassed = 0;
}

/* Bronstein delay switch callback */
void bronstein_switch_callback()
{
	if (delayPassed)
	{
		add_time(playerTime[currentPlayer], gameConfig.delay);	
		delayPassed = 0;	
	}
	else
	{
		for (uint8_t i = 0; i < 6; i++) playerTime[currentPlayer][i] = startTime[i];
		playerTicks[currentPlayer] = startTicks;		
	}
	
	for (uint8_t i = 0; i < 6; i++) startTime[i] = playerTime[currentPlayer ^ 0x01][i];
	startTicks = playerTicks[currentPlayer ^ 0x01];
	
	delay = gameConfig.delay; 
	delayTicks = 127;	
	
}

/* Hourglass switch callback */
void hourglass_switch_callback()
{
	
}

/* Count up switch callback */
void countup_switch_callback()
{

}

/* Placeholder tick callback */
void empty_tick_callback()
{
	
}

/* Tick callback for simple and increment modes */
void simple_tick_callback()
{
	on_tick();
}

/* Simple delay tick callback */
void delay_tick_callback()
{
	if (delayPassed) // proceed as normal if delay period has elapsed
	{
		on_tick();	
	}
	else
	{		
		if (++delayTicks > 127)
		{
			delayTicks = 0;		
			
			if (--delay < 0) delayPassed = 1;	
		}		
	}
}

/* Bronstein delay tick callback */
void bronstein_tick_callback()
{
	if (!delayPassed)
	{
		if (++delayTicks > 127)
		{
			delayTicks = 0;	
				
			if (--delay < 0) delayPassed = 1;
		}	
	}
	
	on_tick();
}

/* Hourglass tick callback */
void hourglass_tick_callback()
{
	
}

/* Count up tick callback */
void countup_tick_callback()
{
	
}
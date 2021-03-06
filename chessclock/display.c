/*
 * display.c
 *
 * Created: 04/07/2020 13:46:47
 *  Author: Joshua
 */ 

#include "display.h"
#include "modes.h"

/* 0 = hr/min, 1 = min/sec */
uint8_t hmms=1;

volatile uint8_t displayBuffer[8];

volatile uint8_t blinkOsc[8];
volatile uint8_t blinkMask[8];

uint8_t blinkTimer;
const uint8_t blinkThreshold=20;

/* |d.p.|g|f|e|d|c|b|a| */
const uint8_t sevenSeg[38] = // TODO: PROGMEM?
{
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2 
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01100111, // 9
	0b01110111, // A
	0b01111100, // b
	0b00111001, // C
	0b01011110, // d
	0b01111001, // E
	0b01110001, // F
	0b00111101, // G
	0b01110110, // H
	0b00010001, // i
	0b00001110, // J
	0b01110101, // k
	0b00111000, // L
	0b01010101, // m
	0b01010100, // n
	0b01011100, // o
	0b01110011, // p
	0b01100111, // q
	0b01010000, // r
	0b00101101, // S
	0b01111000, // t
	0b00011100, // u
	0b00101010, // V
	0b01101010, // W
	0b01001001, // X
	0b01101110, // y
	0b00011011, // Z
	0b01010011, // ?
 0b00000000, // space
};

// /* |d.p.|a|b|c|d|e|f|g| */
// const uint8_t sevenSeg[38] = // TODO: PROGMEM
// {
// 	0b01111110, // 0
// 	0b00110000, // 1
// 	0b01101101, // 2
// 	0b01111001, // 3
// 	0b00110011, // 4
// 	0b01011011, // 5
// 	0b01011111, // 6
// 	0b01110000, // 7
// 	0b01111111, // 8
// 	0b01110011, // 9
// 	0b01110111, // A
// 	0b00011111, // b
// 	0b01001110, // C
// 	0b00111101, // d
// 	0b01001111, // E
// 	0b01000111, // F
// 	0b01011110, // G
// 	0b00110111, // H
// 	0b01000100, // i
// 	0b00111000, // J
// 	0b01010111, // k
// 	0b00001110, // L
// 	0b01010101, // m
// 	0b00010101, // n
// 	0b00011101, // o
// 	0b01100111, // p
// 	0b01110011, // q
// 	0b00000101, // r
// 	0b01011010, // S
// 	0b00001111, // t
// 	0b00011100, // u
// 	0b00101010, // V
// 	0b00101011, // W
// 	0b01001001, // X
// 	0b00111011, // y
// 	0b01101100, // Z
// 	0b01100101, // ?
// 	0b00000000, // space
// };

static void tx_spi(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
}

void init_display(uint8_t brightness)
{
	/* SS must be set as output for master mode */
	/* but CS is used as the actual select pin  */
	SPI_DDR |= 1<<SCK | 1<<MOSI | 1<<SS | 1<<CS; 
	SPI_PORT |= 1<<CS;                  
	SPCR = 1<<SPE | 1<<MSTR | 1<<SPR1; // master mode, /64 prescaler (125kHz)
	
	DDRD |= 1<<PD4 | 1<<PD5; // colon, OC0B
	
	COLON_OFF();	
	
	TCCR0A = 1<<COM0B1 | 1<<COM0B0 | 1<<WGM01 | 1<<WGM00; // Fast PWM, TOP = OCR0A
	TCCR0B = 1<<WGM02 | 1<<CS02;                          // /256 prescaler
	TIMSK0 = 1<<OCIE0A;                                   // compare interrupt
	OCR0A = 48;					                          // ~640Hz at 8MHz
	OCR0B = brightness << 2;                              // Display brightness PWM: 1-10 is mapped to 4-40
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x0C);		// shutdown
	tx_spi(0x01);		//
	PORTB |= 1<<CS;
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x0B);		// scanmode
	tx_spi(0x07);		//
	PORTB |= 1<<CS;
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x0A);	    // intensity
	tx_spi(0x0F);		//
	PORTB |= 1<<CS;
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x09);		// decode
	tx_spi(0x00);		// no decode
	PORTB |= 1<<CS;	
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x0F);		// test mode
	tx_spi(0x00);		// off
	PORTB |= 1<<CS;
	
	for (uint8_t i = 0; i < 8; i++)
	{
		blinkOsc[i] = 0x00;
		blinkMask[i] = 0x00;
	}
}

void update_blink(void)
{
	blinkTimer++;
	if (blinkTimer > blinkThreshold)
	{
		blinkTimer = 0;
		for (uint8_t i = 0; i < 8; i++)
		{			
			blinkOsc[i] ^= 0xFF;
		}
	}
}

void write_time(uint8_t edit)
{
	if (hmms) hmms = 2;
	
	if (playerTime[PLAYER_A][hmms] || edit) displayBuffer[0] = sevenSeg[playerTime[PLAYER_A][hmms]];
	else                                    displayBuffer[0] = 0x00;
	
	if (playerTime[PLAYER_B][hmms] || edit) displayBuffer[4] = sevenSeg[playerTime[PLAYER_B][hmms]];
	else                                    displayBuffer[4] = 0x00;

/* put this loop back later */	
// 	for (uint8_t i = 1; i < 4; i++)
// 	{
// 		displayBuffer[i]   = sevenSeg[playerATime[i+hmms]];
// 		displayBuffer[i+4] = sevenSeg[playerBTime[i+hmms]];
// 	}

	displayBuffer[1] = sevenSeg[playerTime[PLAYER_A][1+hmms]] | 0x80; // dp
	displayBuffer[2] = sevenSeg[playerTime[PLAYER_A][2+hmms]];
	displayBuffer[3] = sevenSeg[playerTime[PLAYER_A][3+hmms]];
	
	displayBuffer[5] = sevenSeg[playerTime[PLAYER_B][1+hmms]] | 0x80; // dp
	displayBuffer[6] = sevenSeg[playerTime[PLAYER_B][2+hmms]];
	displayBuffer[7] = sevenSeg[playerTime[PLAYER_B][3+hmms]];
}

void write_char(uint8_t chr, uint8_t pos)
{
	if      (chr >= 65 && chr < 91)  chr -= 55; // uppercase
	else if (chr == ' ')             chr  = 37; // space
	else if (chr >= 97 && chr < 123) chr -= 87; // lowercase
	else if (chr >= 48 && chr < 58)  chr -= 48; // numbers
	else if (chr == '?')             chr  = 36;
	else                             chr  = 37;
	
	displayBuffer[pos] = sevenSeg[chr];
}

void write_string(const char *str, uint8_t start, uint8_t end)
{
	uint8_t i;
	char c;
	
	for (i = start; i < end && (c = str[i-start]) != '\0'; i++)
		write_char(c, i);
	
	for (; i < end; i++)
		displayBuffer[i] = 0x00;
}

void write_number_8(uint8_t num, uint8_t start)
{
	const uint8_t factors[3] = {100, 10, 1};
	uint8_t z = 0;
	
	displayBuffer[start] = 0x00;
	
	for (uint8_t i = 1; i < 4; i++)
	{
		uint8_t q = num / factors[i-1];
		
		if (q == 0)
		{
			if (z || (i == 3)) displayBuffer[start+i] = sevenSeg[0];	
			else   displayBuffer[start+i] = 0x00;		
		}
		else
		{
			displayBuffer[start+i] = sevenSeg[q];
			num -= q*factors[i-1];
			z = 1;	
		}			
	}	
}

ISR(TIMER0_COMPA_vect)
{
	static uint8_t i;
	
	uint8_t dispRows = ~(1<<i); // drive current row low
	uint8_t dispCols = displayBuffer[i] & ~(blinkMask[i] & blinkOsc[i]);
	
	PORTB &= ~(1<<CS);
	tx_spi(dispRows);
	tx_spi(dispCols);
	PORTB |= 1<<CS;
		
	i++;
	i &= 0x07;	
}




